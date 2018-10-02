#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

int main() {
    string input;
    bool cont = true;
    //TODO: Continually prompt for textual user input on a command line
    while (cont) {


        int pid = fork();
        if (pid == 0) {  // child
            exec(cmd);
        }
        else {  // parent
            waitpid(pid);
        }
    }
}
