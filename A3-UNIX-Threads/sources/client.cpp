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

     //Constructor
     dataForThread(int nInp, char *data_string_inp, SafeBuffer *req_buffer_inp) :
        n(nInp), data_string(data_string_inp), req_buffer(req_buffer_inp) {}
     };

struct workerData {    /* Used as argument to thread_start() */
    RequestChannel *work_channel;
    SafeBuffer *req_buffer; //Need to know where to push
    Histogram *hist;

    //Constructor
    workerData(RequestChannel *work_channel_inp, SafeBuffer *req_buffer_inp, Histogram *hist_inp) :
            work_channel(work_channel_inp), req_buffer(req_buffer_inp), hist(hist_inp) {}
};

//This function is fed to the thread as "start_routine"
void* request_thread_function(void* arg) {
    
    dataForThread* data = (dataForThread*)arg;

	for(int i = 0; i < data->n; i++) {
        data->req_buffer->push(data->data_string);
	};
    data->req_buffer->push("quit");
    cout << "Request buffer address we pushed to: "<<data->req_buffer<<"\n";
    cout << "Finished pushing data to request buffer from client\n";
	//Retval is used by the pthread_join() function
	return NULL; //For now
}

void* worker_thread_function(void* arg) {

    workerData* data = (workerData*)arg;


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

    while(data->req_buffer->size() > 0) {
        string request = data->req_buffer->pop();
        cout << "Popping from request buffer, string is "<<request<<"\n";
        data->work_channel->cwrite(request); //Sends "requests" to the server
        string response = data->work_channel->cread();
        data->hist->update(request,response);
    }
}

void pushData(int n, SafeBuffer * request_buffer) {
    //Start 3 local threads here
    pthread_t johnThread;
    pthread_t janeThread;
    pthread_t joeThread;


    dataForThread* john = new dataForThread(n,"data John Smith",request_buffer); //Will be destroyed by the join apparently;
    dataForThread* jane = new dataForThread(n,"data Jane Smith",request_buffer);
    dataForThread* joe  = new dataForThread(n,"data Joe Smith",request_buffer);

    //Create the buffer pushing threads
    pthread_create(&johnThread, NULL, request_thread_function,john);
    pthread_create(&janeThread, NULL, request_thread_function,jane);
    pthread_create(&joeThread, NULL, request_thread_function,joe);

    //Join the buffer pushing threads
    pthread_join(johnThread, NULL);
    pthread_join(janeThread, NULL);
    pthread_join(joeThread, NULL);
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 10; //default number of requests per "patient"
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

		pushData(n, &request_buffer);
        cout << "size of request buffer "<<request_buffer.size()<<"\n";

        vector<RequestChannel*> workerChannels;
        vector<pthread_t> threadIDs;
        for (int i = 0; i < w; i++) {
            cout << "In here\n";
            chan->cwrite("newchannel"); //Used for sending strings to server, other commands: data <data>
            string s = chan->cread (); //cread gets the response
            workerChannels.push_back(new RequestChannel(s, RequestChannel::CLIENT_SIDE));
            threadIDs.push_back(i);
            pthread_create(&threadIDs.at(i), NULL, worker_thread_function,new workerData(workerChannels.at(i),&request_buffer,&hist)); //Last args is null atm.
            //STUCK RIGHT HERE
        }

        cout << "Finished making workerthreads\n";
        for (int i = 0; i < workerChannels.size(); i++) {
            cout << "In there\n";
            pthread_join(threadIDs.at(i), NULL);
            delete workerChannels.at(i);
        }

        //The work the thread does:
        //Pop anything from the buffer
        //Send data it popped
        //If buffer is empty, quit the thread?

        //Update a quit variable, saying how many quits it has seen.
        //If quit == n. Close thread.

        /*
    	//Handshake start
        chan->cwrite("newchannel"); //Used for sending strings to server, other commands: data <data>
        //cwrite is a method in the RequestChannel chan object
        //Response to request newchannel returns a key
		string s = chan->cread (); //cread gets the response
        RequestChannel *workerChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
        //Handshake end
        */
        /*
        while(true) {

            string request = request_buffer.pop();
            cout << "Popping from request buffer, string is "<<request<<"\n";
			workerChannel->cwrite(request); //Sends "requests" to the server

			if(request == "quit") {
			   	delete workerChannel;
                break;
            }else{
				string response = workerChannel->cread();
				hist.update (request, response);
			}
        }
         */
        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl; 

		hist.print ();
    }
}
