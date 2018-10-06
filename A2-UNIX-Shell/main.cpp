#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>
#include <algorithm>

using namespace std;

string quotes = "\"\'";

/***************************************************************************************
*    Title: String tokenizer in C++ w. delimiter characters
*    Author: linello (Stack Overflow username)
*    Date: 10.02.2018
*    Code version: 02.02.2014 14:27
*    Availability: https://stackoverflow.com/questions/9823263/string-tokenization-in-c-including-delimiter-characters
*
***************************************************************************************/
vector<string> tokenizeString(const string& str, const string& delimiters)
{
    vector<string> tokens;
    string::size_type quoteStart = 0;
    string::size_type quoteEnd = 0;
    bool firstQuote = false;
    bool foundEnd = false;

    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0); //Jump over spaces at the beginning
    cout << "lastPos "<<lastPos<<"\n";
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos); //Find first space
    cout << "pos "<<pos<<"\n";


    while (string::npos != pos || string::npos != lastPos)
    {  // Found a token, add it to the vector.
        //When encountering first ", don't do this
        if (str.find_first_of(quotes, quoteStart) != string::npos) {
            if (firstQuote == false) {
                quoteStart = str.find_first_of(quotes, lastPos); //Look from lastPos, and see if there are any quotes. Save quote location as split start
                cout << "quoteStart " << quoteStart<<"\n";
                firstQuote = true;
            } else {
                firstQuote = false;
                foundEnd = true;
                quoteEnd = str.find_first_of(quotes, lastPos+1);
                cout << "quoteEnd "<<quoteEnd<<"\n";
            }
        }
        if (foundEnd) {
            foundEnd = false;
            tokens.push_back(str.substr(quoteStart, quoteEnd+1 - quoteStart));
            cout << "lastPos was set in foundEnd\n";
            lastPos = quoteEnd + 1;
            pos = str.find_first_of(delimiters, lastPos);
        }
        else {
            if (str.substr(lastPos, pos - lastPos).size() > 0) {
                tokens.push_back(str.substr(lastPos, pos - lastPos));
            }
            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of(delimiters, pos);
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }
        cout << "before ending while loop:\n";
        cout << "lastPos: "<<lastPos<<"\n";
        cout << "pos: "<<pos<<"\n";
    }
    cout << "size of tokens: "<<tokens.size()<<"\n";
    return tokens;
}

//Function returns string vector with all special characters and words seperated in seperate strings
//Works on: "abc|cdf xxx|yyy" but not "abc|dfe|xxx"
vector<string> splitBySpecials(vector<string> tokens, const string& specials) {

    bool debug = true;
    vector<string> betterTokens;
    size_t specialCharIndex;
    size_t prevSpesCharIndex = 0;

    if(debug) cout << "We in the game fam\n";
    for (unsigned int i = 0; i < tokens.size(); i++) {
        if(debug) cout << "In for loop at index "<<i<<" looking at token "<<tokens.at(i)<<"\n";

        //Function to make the token splitter ignore everything after a quote
        bool finished = false;
        if (tokens[i].find_first_of(quotes) != string::npos && !finished) { //Does the current token have a "?
            cout << "passed if loop in splitBySpecials\n";
            //Add it to betterTokens
            betterTokens.push_back(tokens[i]);
            for (i = i+1 ; i < tokens.size() && !finished; i++) {
                if (tokens[i].find_first_of(quotes) != string::npos) { //If this token has a "
                    betterTokens.push_back(tokens[i]);
                    finished = true;
                }
                else {
                    betterTokens.push_back(tokens[i]);
                }
            }
        }
        if (finished) {
            continue;
        }



        bool haveSplit = false;

        //While we haven't searched the entire string

        specialCharIndex = tokens[i].find_first_of(specials); //returns string::npos if can't find any
        if(debug) cout << "initial specialCharIndex: "<<specialCharIndex<<"\n";

        if(debug) cout << "Before while loop \n";
        //Adds what is before the token, and the token
        while  (specialCharIndex != string::npos) {
            if(debug)cout << "in dat while loop\n";

                if(debug)cout << "tokens[i].size() "<<tokens[i].size()<<" and tokens[i] == "<<tokens[i]<<"\n";
                if (specialCharIndex > 0) {
                    //add the text before the token
                    if(debug)cout << "pushBack the text before the token:"<<tokens[i].substr(0,specialCharIndex)<<"endhere\n";
                    betterTokens.push_back(tokens[i].substr(0,specialCharIndex)); //normal text, from the start
                }

            if(debug)cout << "pushBack the token: "<<tokens[i].substr(specialCharIndex,1)<<"\n";
            betterTokens.push_back(tokens[i].substr(specialCharIndex,1)); //The actual token
            haveSplit = true;

            prevSpesCharIndex = specialCharIndex;

            //The code below doesn't really make sense with the grammar we have
            specialCharIndex = tokens[i].find_first_of(specials, specialCharIndex+1); //Look for additional special chars

            //Add what is after the token
            //If there is still another special character
            if (specialCharIndex < string::npos) {
                if(debug)cout << "Added everything after the token up to the next token "<<tokens[i].substr(prevSpesCharIndex+1,specialCharIndex-prevSpesCharIndex)<<"\n";
                betterTokens.push_back(tokens[i].substr(prevSpesCharIndex+1,specialCharIndex-prevSpesCharIndex)); //The text between previous and next special char
            }
            else {
                //Don't add empty stirngs
                if (tokens[i].substr(prevSpesCharIndex+1,string::npos).size() >= 1) {
                    if(debug)cout << "Added everything in the string after the token: " << tokens[i].substr(prevSpesCharIndex+1,string::npos)<<"\n";
                    betterTokens.push_back(tokens[i].substr(prevSpesCharIndex+1,string::npos));
                }
            }

        };
        if(debug)cout << "While loop done\n";

        if (haveSplit == false) {
            if(debug)cout << "havesplit == false, added "<<tokens.at(i)<<" to betterTokens\n";
            betterTokens.push_back(tokens.at(i));
        }

    }
    if(debug)cout << "Returning from splitBySpecials!";
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

int normalExecvp(char* arglist[]) {
    int pid = fork();
    if (pid < 0) {
        perror("fork() error");
        exit(-1);
    }
    else if (pid != 0) {  // parent
        cout << "Inside the parent!\n";
        int result = wait(nullptr); //Returns child process ID, or -1 if the child had an error
        //TODO: This dude does a do-while-loop. Why? https://brennan.io/2015/01/16/write-a-shell-in-c/
        //Alternative: waitpid(pid, NULL, 0)
        cout << "Child returned!\n";
        return result;
    }
    else {  // child
        //exec(cmd);
        cout << "Inside the child!\n";
        cout << "the command: "<<arglist[0]<<"\n";
        //char *argz[] = {"ls", "-l",nullptr};
        execvp(arglist[0],arglist);
        //execl("ls","ls","-l",NULL);
    }
}


//Function edits the charStringArray[] that is passed in to the function
void stringVectorToArray(vector<string> arguments, char* charStringArray[]) {

    charStringArray[arguments.size()] = nullptr;
    vector<char*> charVector;

    //Convert vector<string> to vector<char*>
    transform(arguments.begin(), arguments.end(), std::back_inserter(charVector), convert);

    //Convert vector<char*> to char*[]
    for (int i = 0; i < arguments.size(); i++) {
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

bool hasSpecials(vector<string> arguments, string specials, int startIndex) {

    bool ignore = false;

    for (int t = startIndex; t<arguments.size();t++) {
        cout << "token: "<<arguments[t]<<"\n";
        if (arguments[t].find_first_of(quotes) != string::npos ) {
            cout << "Found quotes!\n";
            if (ignore == false) {
                ignore = true;
            }
            else {
                ignore = false;
            }
        }
        if ((arguments[t].find_first_of(specials) != string::npos) && (ignore == false) && arguments[t].find_first_of(quotes) == string::npos) {
            cout << "found special! "<< arguments[t].find_first_of(specials)<<"\n";
            return true;
        }
    }
    return false;
}

int evaluateCommand(vector<string> arguments, string specials) {

    char* arglist[arguments.size()+1];

    stringVectorToArray(arguments,arglist); //No return value, but populates arglist

    //Check if 'cd' or 'exit'
    if (arguments.at(0) == "cd") {
        //Do cd logic
    }
    else if (arguments.at(0) == "exit" || arguments.at(0) == "EXIT") {
        //do exit logic
    }

    int fd[2]; //Just in case of piping
    //fd[0] --Read-end
    //fd[1] --Write-end
    /*
     * "0", with a unistd.h symbolic constant of STDIN_FILENO
     * "1", with a unistd.h symbolic constant of STDOUT_FILENO
     * "2", with a unistd.h symbolic constant of STDERR_FILENO
     */
    //Make a pipe to be used from now on
    pipe(fd);

    //If there is nothing special going on, just run the execvp
    if ( ! hasSpecials(arguments,specials,0) ) { //if there are pipes or redirects
        return normalExecvp(arglist);
    }

    for (int i = 0; i < arguments.size(); i++) {

        cout << "running loop for the "<<i<<"th time, when arguments.size() == "<<arguments.size()<<"\n";

        //Make sure commands that have been executed are not executed again.
        //By moving i to aftet the commands

        //Execute normally. Otherwise, if there are more pipes.
        //continue execution:
        if (arguments.at(i) == "|") {
            vector<string> argsBefore;

            //Pipe logic
            //Put all arguments before pipe symbol in separate vector
            //Make array argsBefore
            for (int j = 0; j < i; j++) {
                argsBefore.push_back(arguments.at(j));
            }

            char* charArrayBfr[argsBefore.size()+1];
            stringVectorToArray(argsBefore,charArrayBfr); //making it ready for execvp

            cout << "Args in that new char array:\n";
            for (int l = 0; l < argsBefore.size(); l++) {
                cout << charArrayBfr[l]<<"\n";
            }

            int pid = fork();
            if (pid < 0) {
                perror("fork() error");
                exit(-1);
            }
            else if (pid != 0) {  // parent
                cout << "Inside the parent!\n";
                //Wait until child has finished with the pipe
                int result = wait(nullptr); //Returns child process ID, or -1 if the child had an error
                cout << "Child returned!\n";
            }
            else {  // child
                cout << "Inside the child!\n";
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
            cout << "Printing out arguments added to argsAfter\n";
            for (int j = i; j < arguments.size(); j++) {
                if (arguments.at(i) != "|") {
                    argsAfter.push_back(arguments.at(j));
                    cout << arguments.at(j)<<"\n";
                }
                else {
                    cout << "Hit another pipe, stop adding to argsAfter.\n";
                    i = i+1; //This line of code is here to make sure we don't run the code again.
                    break;
                }
            }

            char* charArrayAfter[argsAfter.size()+1];
            stringVectorToArray(argsAfter,charArrayAfter); //making it ready for execvp

            cout << "Args in that new char array:\n";
            for (int l = 0; l < argsAfter.size(); l++) {
                cout << charArrayAfter[l]<<"\n";
            }

            int pid = fork();
            if (pid < 0) {
                perror("fork() error");
                exit(-1);
            }
            else if (pid != 0) {  // parent
                cout << "Inside the parent nr. 2!\n";
                //Wait until child has finished with the pipe
                int result = wait(nullptr); //Returns child process ID, or -1 if the child had an error
                cout << "Child nr. 2 returned!\n";
            }
            else {  // child
                cout << "Inside the child nr. 2!\n";
                //close(fd[0]); //Read-end
                //Read input from pipe read end
                dup2(fd[0], STDIN_FILENO); //make stdin fd[0] (read-end)

                //If the next argument is a pipe
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

    //normalExecvp(arglist);
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

        tokens = tokenizeString(input,delim);
        //tokens = splitBySpecials(tokens,specials);

        //evaluateCommand(tokens,specials);
        testFunction(tokens,specials);
        cout << "> ";
    }
}





