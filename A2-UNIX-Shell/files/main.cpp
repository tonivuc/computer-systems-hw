#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <fcntl.h>
#include <zconf.h>

using namespace std;

string quotes = "\"\'";

vector<int> bgProcessIDs;
bool backgroundProcces = false;


//Splits by space, but keeps stuff in quotes together. hm
//Known bug: A quote starting with " can be closed by '. This can be solved with stack based approach.
vector<string> tokenizeString(const string& str, const string& delimiters)
{

    vector<string> tokens;
    bool firstRun = true;

    // Skip delimiters at beginning.
    string::size_type lastPos = 0;
    // Find first "non-delimiter".
    string::size_type pos = 0;


    do {
        if (!firstRun) {
            tokens.push_back(str.substr(lastPos, pos - lastPos));
        }
        else { firstRun = false; }

        lastPos = str.find_first_not_of(delimiters, pos); //Find next non-delimiter character

        if (string::npos == pos && string::npos == lastPos || pos > str.length()-1) break; //Stop execution if end of string is reached

        if (str.substr(lastPos,1) == "\"" || str.substr(lastPos,1) == "\'") {
            pos = str.find_first_of(quotes, lastPos+1)+1;

        }
        else {
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }
    } while (string::npos != pos || string::npos != lastPos);

    return tokens;
}

vector<string> removeOccurancesOf(vector<string> arguments, char toRemove) {
    for (int i = 0; i < arguments.size();i++) {

        arguments[i].erase(std::remove(arguments[i].begin(), arguments[i].end(), toRemove), arguments[i].end());

    }
    return arguments;
}

bool isBackgroundProcessOldFunc(vector<string> arguments) {
    if (arguments[arguments.size()-1] == "&") {
        return true;
    } else {
        return false;
    }
}

bool isBackgroundProcess(vector<string> args) {
    return backgroundProcces;
}

//Function returns string vector with all special characters and words seperated in seperate strings
//Works on: "abc|cdf xxx|yyy" but not "abc|dfe|xxx"
vector<string> splitBySpecials(vector<string> tokens, const string& specials) {

    bool debug = false;
    vector<string> betterTokens;
    size_t specialCharIndex;
    size_t prevSpesCharIndex = 0;

    for (unsigned int i = 0; i < tokens.size(); i++) {

        //Have to ignore tokens which include " or '
        if (tokens[i].find_first_of(quotes) != string::npos) {
            betterTokens.push_back(tokens[i]);
            continue;
        }

        bool haveSplit = false;

        specialCharIndex = tokens[i].find_first_of(specials); //returns string::npos if can't find any

        //Adds what is before the token, and the token
        while  (specialCharIndex != string::npos) {

                if (specialCharIndex > 0) {
                    //add the text before the token";
                    betterTokens.push_back(tokens[i].substr(0,specialCharIndex)); //normal text, from the start
                }

            betterTokens.push_back(tokens[i].substr(specialCharIndex,1)); //The actual token
            haveSplit = true;

            prevSpesCharIndex = specialCharIndex;

            //The code below doesn't really make sense with the grammar we have
            specialCharIndex = tokens[i].find_first_of(specials, specialCharIndex+1); //Look for additional special chars

            //Add what is after the token
            //If there is still another special character
            if (specialCharIndex < string::npos) {
                betterTokens.push_back(tokens[i].substr(prevSpesCharIndex+1,specialCharIndex-prevSpesCharIndex)); //The text between previous and next special char
            }
            else {
                //Don't add empty stirngs
                if (tokens[i].substr(prevSpesCharIndex+1,string::npos).size() >= 1) {
                    betterTokens.push_back(tokens[i].substr(prevSpesCharIndex+1,string::npos));
                }
            }

        };

        if (haveSplit == false) {
            betterTokens.push_back(tokens.at(i));
        }
    }
    return betterTokens;
}


/***************************************************************************************
*    Title: std::vector<std::string> to char* array
*    Author: Nawaz (Stack Overflow username)
*    Date: 10.02.2018
*    Code version: 08.13.2011 07:36
*    Availability: https://stackoverflow.com/questions/7048888/stdvectorstdstring-to-char-arrays
*
***************************************************************************************/
char *convert(const std::string & s)
{
    char *pc = new char[s.size()+1];
    strcpy(pc, s.c_str());
    return pc;
}

int normalExecvp(char* arglist[], bool isBGProcess) {
    int pid = fork();
    if (pid < 0) {
        perror("fork() error");
        exit(-1);
    }
    else if (pid != 0) {  // parent
        int result = 0;
        if (!isBGProcess) {
            result = wait(nullptr); //Returns child process ID, or -1 if the child had an error
        }
        else {
            bgProcessIDs.push_back(pid);
            cout << "pushing process ID "<<pid<<" to process ID vector\n";
        }
        return result;
    }
    else {  // child
        execvp(arglist[0],arglist);
        //execl("ls","ls","-l",NULL);
    }
}


//Function converts an array of strings to an array of char pointers with char strings
void stringVectorToArray(vector<string> arguments, char* charStringArray[]) {

    charStringArray[arguments.size()] = nullptr;
    vector<char*> charVector;

    //Convert vector<string> to vector<char*>
    transform(arguments.begin(), arguments.end(), std::back_inserter(charVector), convert);

    //Convert vector<char*> to char*[]
    int i = 0;
    for ( ; i < arguments.size(); i++) {
        //cout << "i: "<<i<<"\n";
        charStringArray[i] = charVector[i];
        //cout << "added this to char array:"<<charStringArray[i]<<"checking for whitespace\n";
    }
}

//Function converts an array of strings to an array of char pointers with char strings
void stringVectorToArray(vector<string> arguments, char* charStringArray[], int lastIndexToCopy) {

    charStringArray[lastIndexToCopy+1] = nullptr;
    vector<char*> charVector;

    //Convert vector<string> to vector<char*>
    transform(arguments.begin(), arguments.end(), std::back_inserter(charVector), convert);

    //Convert vector<char*> to char*[]
    int i = 0;
    for ( ; i <= lastIndexToCopy; i++) {
        charStringArray[i] = charVector[i];
    }

}

//This function checks if the process should write to standard out or not
//The alternative would be that the process writes to the pipe
bool writeToPipe(vector<string> arguments, int searchStartIndex) {
    bool morePipes = false;
    for (int y = searchStartIndex; y < arguments.size(); y++) {
        if (arguments.at(y) == "|") {
            morePipes = true;
            break;
        }
    }
    return morePipes;
}

//Used to search the vector of strings for special characters, ignoring those within quotes
bool hasSpecials(vector<string> arguments, string specials, int startIndex) {

    bool ignore = false;

    for (int t = startIndex; t<arguments.size();t++) { ;
        if (arguments[t].find_first_of(quotes) != string::npos ) {
            if (ignore == false) {
                ignore = true;
            }
            else {
                ignore = false;
            }
        }
        if ((arguments[t].find_first_of(specials) != string::npos) && (ignore == false) && arguments[t].find_first_of(quotes) == string::npos) { ;
            return true;
        }
    }
    return false;
}


void splitBasedOnRedir(vector<string> argsIn, char** argsOut) {
    int i = 0;
    for ( ; i < argsIn.size(); i++) {
        if (argsIn[i][0] == '>' || argsIn[i][0] == '<') {
            break;
        }
    }

    stringVectorToArray(argsIn, argsOut, i);
}



char findRedirectType(vector<string> arguments) {
    string symbols = "<>";

    for (int i = 0; i < arguments.size();i++) {

        if (arguments[i].find_first_of(symbols) != string::npos) {
            assert(arguments[i].size() == 1);
            char character = arguments[i][0];
            return character;
        }
    }
    return 0;
}

int findFirstArgWith(vector<string> args, string searchChars, int startIndex) {
    for (int i = startIndex; i < args.size();i++) {
        if (args[i].find_first_of(searchChars) != string::npos) {
            return i;
        }
    }
    return -1;
}

void killZombies() {
    int status;
    int finishedChild;
    for (int i = 0; i < bgProcessIDs.size(); i++) {
        //finishedChild = wait(nullptr);
        finishedChild = waitpid(bgProcessIDs[i],&status,WNOHANG);
        //kill(bgProcessIDs[i], SIGKILL);
        if (finishedChild > 0) {
            bgProcessIDs.erase(bgProcessIDs.begin()+i);
            cout << "Process " << finishedChild << " has reaped from background processes.\n";
        }
    }
}

//Return -1 on failure, 0 otherwise
//1st input only contains the arguments to be executed
int executeRedirect(char** charArrayBfr, char redirType, int fd[], int fileFD, bool isBGProcess) {

    int pid = fork();
    if (pid < 0) {
        perror("fork() error");
        exit(-1);
    }
    else if (pid != 0) {  // parent
        int result = 0;
        if (!isBGProcess) {
            result = wait(nullptr); //Returns child process ID, or -1 if the child had an error
        }
        else {
            bgProcessIDs.push_back(pid);
        };
        return result;
    }
    else {  // child
        //close(fd[0]); //Read-end

        if (redirType == '>') {
            if (fd[0] < 0) { //If we don't read from the pipe
                dup2(fileFD, STDOUT_FILENO); //make stdout point to the new file
            }
            else { //Read from the read-end of pipe
                dup2(fd[0], STDIN_FILENO); //make stdin point to pipe read-end
                dup2(fileFD, STDOUT_FILENO); //make stdout point to the new file
            }
        }
        else if (redirType == '<') {
            //do something else
        }
        //close(fd[1]);
        execvp(charArrayBfr[0],charArrayBfr);
    }
    return -1;
}

int doRedirect(vector<string> arguments, bool closeFileAfter, int pipeFD[], int &fileFD, bool beforePipe, bool &keepFileOpen) {

    char redirType = findRedirectType(arguments);
    if (redirType != 0 && findFirstArgWith(arguments,"|",0) == -1) {
        int redirIndex = findFirstArgWith(arguments,"<>",0);
        char* charArrayRedir[redirIndex+1];
        stringVectorToArray(arguments,charArrayRedir,redirIndex-1);

        if (redirType == '>') {
            fileFD = open(arguments[redirIndex+1].c_str(), O_CREAT|O_WRONLY, S_IRUSR | S_IWUSR); //Create a file if not there, write only, permission flags at end
            if (beforePipe) {
                int noPipe[2] = {-1,-1};
                pipeFD = noPipe;
            }

            int retVal = executeRedirect(charArrayRedir, redirType, pipeFD, fileFD, isBackgroundProcess(arguments));

            if (closeFileAfter) {
                close(fileFD);
            }
            else {
                keepFileOpen = true;
            }
            return retVal;
        }
    }
    else {
        return -1;
    }
}


int evaluateCommand(vector<string> arguments, string specials, int *fd) {

    if (isBackgroundProcessOldFunc(arguments)) {
        arguments.pop_back();
        backgroundProcces = true;
    }

    char* arglist[arguments.size()+1];
    int alreadyExecutedIndex = 0;
    bool fileOpen = false;
    int fileFD = -1;

    stringVectorToArray(arguments,arglist); //No return value, but populates arglist

    pipe(fd);

    //Check if 'cd' or 'exit'
    //Took some code from here for this one: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
    if (arguments.at(0) == "cd") {
        char cwd[PATH_MAX];
        int retVal;
        if (arguments.at(1).c_str() == nullptr) {
            fprintf(stderr, "expected argument: \"cd\"\n");
            return -1;
        } else {
            if (arguments.at(1) == "-") {
                retVal = chdir("..");
            } else {
                retVal = chdir(arguments.at(1).c_str());
            }
            if (retVal<0) {
                cout << "Something is wrong with that path. Maybe it doesn't exist?\n";
            }
            else {
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    cout <<cwd<< " ";
                } else {
                    perror("getcwd() error");
                    return -1;
                }
            }
            return retVal;
        }
    }
    else if (arguments.at(0) == "exit" || arguments.at(0) == "EXIT") {
        exit(0);
    }
    else if (arguments.at(0) == "jobs") {
        cout << "Current background processes: \n";
        for (int i : bgProcessIDs) {
            cout << i <<"\n";
        }
    }

    //Just in case of piping
    //fd[0] --Read-end
    //fd[1] --Write-end
    /*
     * "0", with a unistd.h symbolic constant of STDIN_FILENO
     * "1", with a unistd.h symbolic constant of STDOUT_FILENO
     * "2", with a unistd.h symbolic constant of STDERR_FILENO
     */
    //Make a pipe to be used from now on


    //If there is nothing special going on, just run the execvp
    if ( ! hasSpecials(arguments,specials,0) ) { //if there are pipes or redirects?
        return normalExecvp(arglist, isBackgroundProcess(arguments));
    }

    //Check for redirects in case of no pipes
    if (findFirstArgWith(arguments,"\"\'",0) == string::npos) {
        doRedirect(arguments,true,fd,fileFD,true,fileOpen);
    }


    for (int i = 0; i < arguments.size(); i++) {

        //Make sure commands that have been executed are not executed again.
        //By moving i to aftet the commands

        //Execute normally. Otherwise, if there are more pipes.
        //continue execution:
        if (arguments.at(i) == "|" && alreadyExecutedIndex < i) {
            vector<string> argsBefore;

            //FUNNY. Bug saved me.
            //We don't want to run the commands we already ran, again. This is now in the if-condition.
            for (int j = alreadyExecutedIndex; j < i; j++) {
                argsBefore.push_back(arguments.at(j));
            }

            char* charArrayBfr[argsBefore.size()+1];
            stringVectorToArray(argsBefore,charArrayBfr); //making it ready for execvp

            //Redirect code goes here
            //If it is run, only need to set the dup2 and not run any more forks here
            char redirType = findRedirectType(argsBefore);

            if ( doRedirect(argsBefore, false, fd,fileFD, true, fileOpen) > -1 ) {
                close(fileFD);
                return -1; //ABORT because this type of redirect isn't supported.
            }

            int pid = fork();
            if (pid < 0) {
                perror("fork() error");
                exit(-1);
            }
            else if (pid != 0) {  // parent
                //Wait until child has finished with the pipe
                int result = 0;
                if (!isBackgroundProcess(arguments)) {
                    result = wait(nullptr); //Returns child process ID, or -1 if the child had an error
                }
                else {
                    bgProcessIDs.push_back(pid);
                }
                alreadyExecutedIndex = i;
                //TODO: Possibly add a continue; here?
            }
            else {  // child
                //close(fd[0]); //Read-end

                //Output to pipe write-end
                dup2(fd[1], STDOUT_FILENO); //make stdout fd[1] (write-end of pipe)
                //close(fd[1]);
                execvp(charArrayBfr[0],charArrayBfr);
            }
        }

        //If the previous token was a pipe
        //Run the code, but set the input to be the read end of the pipe
        //And if there are no more pipes, set output to standard out
        if ((i > 0) && arguments.at(i-1) == "|") {
            vector<string> argsAfter;

            //Pipe logic
            //Put all arguments before pipe symbol in separate vector

            for (int j = i; j < arguments.size(); j++) {
                if (arguments.at(j) != "|") {
                    argsAfter.push_back(arguments.at(j));
                }
                else {
                    alreadyExecutedIndex = j;
                    //i = i+1; //This line of code is here to make sure we don't run the code again.
                    break;
                }
            }

            char* charArrayAfter[argsAfter.size()+1];
            stringVectorToArray(argsAfter,charArrayAfter); //making it ready for execvp

            //Takes the arguments after the pipe, and makes sure they are ran!
            if (findFirstArgWith(argsAfter,"\"\'",0) == string::npos) {

                int retVal = doRedirect(argsAfter,true,fd,fileFD,false,fileOpen);
                if (retVal > -1) {
                    return retVal;
                }

            }

            int pid = fork();
            if (pid < 0) {
                perror("fork() error");
                exit(-1);
            }
            else if (pid != 0) {  // parent
                //Wait until child has finished with the pipe

                int result = 0;
                if (!isBackgroundProcess(arguments)) {
                    result = wait(nullptr); //Returns child process ID, or -1 if the child had an error
                }
                else {
                    bgProcessIDs.push_back(pid);
                }

                if (fileOpen) {
                    close(fileFD);
                    fileOpen = false;
                }
                //TODO: No return value in several spots. Why?
            }
            else {  // child
                //close(fd[0]); //Read-end
                //Read input from pipe read end
                dup2(fd[0], STDIN_FILENO); //make stdin fd[0] (read-end)

                //If the next argument is a pipe
                //TODO: Investigate if this shoudl say "arguments" yup
                if ( writeToPipe(arguments,i) ) {
                    dup2(fd[1], STDOUT_FILENO); //make stdout fd[1] (write-end of pipe)
                }
                //If no more pipes
                else {
                    dup2(STDOUT_FILENO,fd[1]); //make fd[1] (write-end of pipe) point to stdout
                }
                //close(fd[1]);

                execvp(charArrayAfter[0],charArrayAfter);
            }
        }


        //Redirect logic will be here
    }

    return -1;
}

vector<string> testFunction(vector<string> arguments, const string& specials) {

    cout << "Printing original tokenized arguments:\n";
    for (string s : arguments) {
        cout << s << "\n";
    }

    vector<string> betterArgs;

    betterArgs = splitBySpecials(arguments,specials);

    cout << "Printing betterTokens\n";

    for (string s : betterArgs) {
        cout << s << "\n";
    }

    return betterArgs;

}


int main() {
    string input;
    std::vector<char*>  charArray;
    const string delim = " ";
    const string specials = "|<>";
    vector<string> tokens;
    int status;;

    cout << "Enter command: ";
    while ( getline( cin, input )) {

        killZombies();

        tokens = tokenizeString(input,delim);
        tokens = splitBySpecials(tokens,specials);
        tokens = removeOccurancesOf(tokens,'\'');

        int fd[2];

        evaluateCommand(tokens,specials,fd);
        backgroundProcces = false; //Reset this
        //testFunction(tokens,specials);
        killZombies();

        //Close the pipe that gets opened every time we run a command.
        for (int i = 0; i < 2;i++) {
            if (close(fd[i])!=0) {
                cout << "error closing fd["<<i<<"]\n";
            }
        }
        cout << "> ";
    }
}





