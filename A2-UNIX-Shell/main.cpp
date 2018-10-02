#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

/***************************************************************************************
*    Title: String tokenizer in C++ w. delimiter characters
*    Author: linello (Stackoverflow username)
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

int executeCommand() {
    cout << "Inside the executeCommand!\n";
    int status;

    int pid = fork();
    if (pid < 0) {
        perror("fork() error");
        exit(-1);
    }
    else if (pid != 0) {  // parent
        cout << "Inside the parent!\n";
        wait(&status); //Stores childs return value
        cout << "Child returned!\n";
        return status;
    }
    else {  // child
        //exec(cmd);
        cout << "Inside the child!\n";
        execvp("ls",nullptr);
        //execl("ls","ls","-l",NULL);
        //Alternative: waitpid(pid, NULL, 0)
    }
    return -1;
}

int main() {
    string input;
    const string delim = " ";
    vector<string> tokens;
    int status;;

    cout << "Enter command: ";
    while ( getline( cin, input )) {

        tokens = tokenizeString(input,delim);
        //cout << input;

        for(string i : tokens) {
            // process i
            cout << i << " "; // this will print all the contents of *features*
        }

        executeCommand();

    }
}



