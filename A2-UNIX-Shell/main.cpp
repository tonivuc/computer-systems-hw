#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main() {
    string input;
    int status;
    bool cont = true;
    //TODO: Continually prompt for textual user input on a command line
    cout << "Enter command: ";
    while ( getline( cin, input )) {

        cout << input;


        int pid = fork();
        if (pid == 0) {  // child
            //exec(cmd);
        }
        else {  // parent
            wait(&status); //Stores childs return value
            //Alternative: waitpid(pid, NULL, 0)
        }
        cont = false;
    }
}
