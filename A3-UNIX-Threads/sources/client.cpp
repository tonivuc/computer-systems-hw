/*
    Based on original assignment by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */


#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#include <sys/time.h>
#include <cassert>
#include <assert.h>

#include <cmath>
#include <numeric>
#include <algorithm>

#include <list>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "reqchannel.h"
#include "SafeBuffer.h"
#include "Histogram.h"
using namespace std;

struct dataForThread {    /* Used as argument to thread_start() */
     int     n;       
     char    *data_string;
     SafeBuffer *req_buffer; //Need to know where to push
     };


//This function is fed to the thread as "start_routine"
void* request_thread_function(void* arg) {
    
    struct dataForThread * data = (struct dataForThread*)arg;
    SafeBuffer request_buffer = *data->req_buffer;
	/*
		Fill in this function.

		The loop body should require only a single line of code.
		The loop conditions should be somewhat intuitive.

		In both thread functions, the arg parameter
		will be used to pass parameters to the function.
		One of the parameters for the request thread
		function MUST be the name of the "patient" for whom
		the data requests are being pushed: you MAY NOT
		create 3 copies of this function, one for each "patient".
	 */

	for(int i = 0; i < data->n; i++) {
        request_buffer.push(data->data_string);
	};
	//Retval is used by the pthread_join() function
	return NULL; //For now
}

void pushUsingThreads(SafeBuffer *buffer, int n) {
    
    struct thread_info {    /* Used as argument to thread_start() */
         pthread_t thread_id;        /* ID returned by pthread_create() */
         int       thread_num;       /* Application-defined thread # */
         char     *argv_string;      /* From command-line argument */
     };
    
    SafeBuffer request_buffer = *buffer;
    
    for(int i = 0; i < n; ++i) {
        request_buffer.push("data John Smith");
        request_buffer.push("data Jane Smith");
        request_buffer.push("data Joe Smith");
    }
    
    
    
}

void* worker_thread_function(void* arg) {
    /*
		Fill in this function. 

		Make sure it terminates only when, and not before,
		all the requests have been processed.

		Each thread must have its own dedicated
		RequestChannel. Make sure that if you
		construct a RequestChannel (or any object)
		using "new" that you "delete" it properly,
		and that you send a "quit" request for every
		RequestChannel you construct regardless of
		whether you used "new" for it.
     */

    while(true) {

    }
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 100; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:w:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function
                break;
			}
    }

    int pid = fork();
	if (pid == 0){
		execl("dataserver", (char*) NULL);
	}
	else {

        cout << "n == " << n << endl;
        cout << "w == " << w << endl;

        cout << "CLIENT STARTED:" << endl;
        cout << "Establishing control channel... " << flush; //What is this? endl. Forces it to be printed immediately.
        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE); //Have to specify client side so it knows how the object will work
        cout << "done." << endl<< flush;

		SafeBuffer request_buffer;
		Histogram hist;

        for(int i = 0; i < n; ++i) {
            request_buffer.push("data John Smith");
            request_buffer.push("data Jane Smith");
            request_buffer.push("data Joe Smith");
        }
        cout << "Done populating request buffer" << endl;

        cout << "Pushing quit requests... ";
        for(int i = 0; i < w; ++i) {
            request_buffer.push("quit");
        }
        cout << "done." << endl;

    	//Handshake start
        chan->cwrite("newchannel"); //Used for sending strings to server, other commands: data <data>
        //cwrite is a method in the RequestChannel chan object
        //Response to request newchannel returns a key
		string s = chan->cread (); //cread gets the response
        RequestChannel *workerChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
        //Handshake end

        while(true) {
            string request = request_buffer.pop();
			workerChannel->cwrite(request); //Sends "requests" to the server

			if(request == "quit") {
			   	delete workerChannel;
                break;
            }else{
				string response = workerChannel->cread();
				hist.update (request, response);
			}
        }
        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl; 

		hist.print ();
    }
}
