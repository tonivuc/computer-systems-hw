#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>
#include <algorithm>

using namespace std;

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
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {  // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
    return tokens;
}

//Function returns string vector with all special characters and words seperated in seperate strings
//NOTE: Function is not perfect. Only works on simple strings like command|command or command>command, but not abc/c(command|command)
vector<string> splitBySpecials(vector<string> tokens, const string& specials) {
    vector<string> betterTokens;
    size_t specialCharIndex;
    size_t prevSpesCharIndex = 0;

    cout << "We in the game fam\n";
    for (unsigned int i = 0; i < tokens.size(); i++) {
        cout << "in that loop\n";

        bool haveSplit = false;

        //While we haven't searched the entire string
        specialCharIndex = tokens[i].find_first_of(specials); //returns string::npos if can't find any
        cout << string::npos << " is string npos\n";
        cout << "dat specialcharIndex is " << specialCharIndex << "\n";

        const size_t numNotFound = string::npos;
        cout << "got over dat hump hm \n";
        while  (specialCharIndex != string::npos) {
            cout << "in dat while loop\n";

            if (haveSplit) {
                betterTokens.push_back(tokens[i].substr(prevSpesCharIndex+1,specialCharIndex-1)); //The text between previous and next special char
            }
            else {
                cout << "pushBack that token!\n";
                betterTokens.push_back(tokens[i].substr(0,specialCharIndex-1)); //normal text, from the start
            }
            betterTokens.push_back(tokens[i].substr(specialCharIndex,1)); //The actual token
            haveSplit = true;
            //While there are still special elements


            prevSpesCharIndex = specialCharIndex;

            specialCharIndex = tokens[i].find_first_of(specials, specialCharIndex+1); //Look for additional special chars
        };

        if (haveSplit == false) {
            betterTokens.push_back(tokens.at(i));
        }

    }
    cout << "Returning from splitBySpecials!";
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



int evaluateCommand(vector<string> arguments) {

    char* arglist[arguments.size()+1];
    arglist[arguments.size()] = nullptr;
    vector<char*> charVector;

    //Convert vector<string> to vector<char*>
    transform(arguments.begin(), arguments.end(), std::back_inserter(charVector), convert);

    //Convert vector<char*> to char*[]
    for (int i = 0; i < arguments.size(); i++) {
        arglist[i] = charVector[i];
    }

    /* REALLY FUNKY STUFF HAPPENS IF I UNCOMMENT THIS
     * Somehow the cout line produces a memory leak
    cout << "Printing arglist: \n";
    for(char* i : arglist) {
        // process i
        cout << "before\n";
        //cout << i << "\n";
        cout << "hello\n";
    }
    cout << "Done printing arglist: \n";
    */

    //Regarding pipes, the entire process is copied. And child and parent share the

    int status;
    normalExecvp(arglist);
    return -1;
}

vector<string> testFunction(vector<string> arguments, const string& specials) {

    vector<string> betterArgs;

    betterArgs = splitBySpecials(arguments,specials);
    /*
    for (string s : betterArgs) {
        cout << s << "\n";
    }
    */
}


int main() {
    string input;
    std::vector<char*>  charArray;
    const string delim = " ,";
    const string specials = "|<>";
    vector<string> tokens;
    int status;;

    cout << "Enter command: ";
    while ( getline( cin, input )) {

        tokens = tokenizeString(input,delim);

        evaluateCommand(tokens);
        //testFunction(tokens,specials);
        cout << "> ";
    }
}





