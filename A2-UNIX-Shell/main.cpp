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
//Works on: "abc|cdf xxx|yyy" but not "abc|dfe|xxx"
vector<string> splitBySpecials(vector<string> tokens, const string& specials) {

    bool debug = false;
    vector<string> betterTokens;
    size_t specialCharIndex;
    size_t prevSpesCharIndex = 0;

    if(debug) cout << "We in the game fam\n";
    for (unsigned int i = 0; i < tokens.size(); i++) {
        if(debug) cout << "In for loop at index "<<i<<" looking at token "<<tokens.at(i)<<"\n";

        bool haveSplit = false;

        //While we haven't searched the entire string
        specialCharIndex = tokens[i].find_first_of(specials); //returns string::npos if can't find any
        if(debug) cout << "initial specialCharIndex: "<<specialCharIndex<<"\n";

        if(debug) cout << "Before while loop \n";
        //Adds what is before the token, and the token
        while  (specialCharIndex != string::npos) {
            if(debug)cout << "in dat while loop\n";

                if (tokens[i].size() >= 1) {
                    //add the text before the token
                    if(debug)cout << "pushBack the text before the token: "<<tokens[i].substr(0,specialCharIndex)<<"\n";
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

bool hasSpecialCommand(vector<string> arguments) {
    for (string s : arguments) {

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

    //Check if 'cd' or 'exit'
    if (arguments.at(0) == "cd") {
        //Do cd logic
    }
    else if (arguments.at(0) == "exit" || arguments.at(0) == "EXIT") {
        //do exit logic
    }

    vector<string> argsBefore;

    for (int i = 0; i < arguments.size(); i++) {
        if (arguments.at(i) == "|") {

            //Pipe logic
            //Put all arguments before pipe symbol in separate vector
            for (int j = 0; j < i; j++) {
                argsBefore.push_back(arguments.at(i));
            }


        }
        //Redirect logic will be here
    }
    normalExecvp(arglist);
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
    const string delim = " ,";
    const string specials = "|<>";
    vector<string> tokens;
    int status;;

    cout << "Enter command: ";
    while ( getline( cin, input )) {

        tokens = tokenizeString(input,delim);
        tokens = splitBySpecials(tokens,specials);

        evaluateCommand(tokens);
        //testFunction(tokens,specials);
        cout << "> ";
    }
}





