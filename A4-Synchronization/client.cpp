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
#include "BoundedBuffer.h"
#include "Histogram.h"
using namespace std;

struct dataForThread {    /* Used as argument to thread_start() */
    int     n;
    char    *data_string;
    BoundedBuffer *req_buffer; //Need to know where to push

    //Constructor
    dataForThread(int nInp, char *data_string_inp, BoundedBuffer *req_buffer_inp) :
            n(nInp), data_string(data_string_inp), req_buffer(req_buffer_inp) {}
};

struct workerData {    /* Used as argument to thread_start() */
    RequestChannel *work_channel;
    BoundedBuffer *req_buffer; //Need to know where to push
    Histogram *hist;

    //Constructor
    workerData(RequestChannel *work_channel_inp, BoundedBuffer *req_buffer_inp, Histogram *hist_inp) :
            work_channel(work_channel_inp), req_buffer(req_buffer_inp), hist(hist_inp) {} ///NANI???
};

struct histogramData {    /* Used as argument to thread_start() */
    RequestChannel *work_channel;
    BoundedBuffer *hist_buffer[3]; //ACTUALLY an array
    Histogram *hist;

    //Constructor
    histogramData(RequestChannel *work_channel_inp, BoundedBuffer *hist_buffer_inp[3], Histogram *hist_inp) :
            work_channel(work_channel_inp), hist(hist_inp) {

        hist_buffer[0] = hist_buffer_inp[0];
        hist_buffer[1] = hist_buffer_inp[1];
        hist_buffer[2] = hist_buffer_inp[2];
    }
};

//This function is fed to the thread as "start_routine"
void* request_thread_function(void* arg) {

    dataForThread* data = (dataForThread*)arg;

    for(int i = 0; i < data->n; i++) {
        data->req_buffer->push(data->data_string);
    };
    cout << "Finished pushing data to request buffer from client\n";
    //Retval is used by the pthread_join() function
    return NULL; //For now
}

void* worker_thread_function(void* arg) {

    workerData* data = (workerData*)arg;

    bool run = true;

    while(run) {
        string request = data->req_buffer->pop();
        data->work_channel->cwrite(request); //Sends "requests" to the server
        if(request == "quit") {
            run = false;
        }
        else {
            string response = data->work_channel->cread();
            data->hist->update(request, response);
        }
    }
    cout << "Quitting worker-thread\n";
    return NULL;
}

void* stat_thread_function(void* arg) {

    histogramData* data = (histogramData*)arg;
    /*
		Fill in this function.

		There should 1 such thread for each person. Each stat thread
        must consume from the respective statistics buffer and update
        the histogram. Since a thread only works on its own part of
        histogram, does the Histogram class need to be thread-safe????

     */

    for(;;) {

    }
}

void pushData(int n, BoundedBuffer * request_buffer) {
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

    delete john;
    delete jane;
    delete joe;
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

    struct timeval start, end;

    int n = 10000; //default number of requests per "patient"
    int w = 100; //default number of worker threads
    int b = 200;
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:w:b:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function
                break;
            case 'b':
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

        //Making BoundedBuffers
        BoundedBuffer request_buffer(b);

        //Store pointers to the response buffers in an array
        BoundedBuffer responseBufferJohn(b/3);
        BoundedBuffer responseBufferJane(b/3);
        BoundedBuffer responseBufferJoe(b/3);
        //Array of BoundedBuffer pointers
        BoundedBuffer *responseBuffers[3]; //Pass this to histogram-threads
        responseBuffers[0] = &responseBufferJohn;
        responseBuffers[1] = &responseBufferJane;
        responseBuffers[2] = &responseBufferJoe;




        Histogram hist;
        //pushData(n, &request_buffer);



        cout << "size of request buffer "<<request_buffer.size()<<"\n";

        //Timing:
        gettimeofday(&start, NULL);


        //Add the Request Threads
        pthread_t johnThread;
        pthread_t janeThread;
        pthread_t joeThread;

        dataForThread* john = new dataForThread(n,"data John Smith",&request_buffer);
        dataForThread* jane = new dataForThread(n,"data Jane Smith",&request_buffer);
        dataForThread* joe  = new dataForThread(n,"data Joe Smith",&request_buffer);


        //Create the buffer pushing threads
        pthread_create(&johnThread, NULL, request_thread_function,john);
        pthread_create(&janeThread, NULL, request_thread_function,jane);
        pthread_create(&joeThread, NULL, request_thread_function,joe);

        //Create worker threads and channels
        vector<RequestChannel*> workerChannels;
        vector<pthread_t> threadIDs;
        vector<workerData*> workerDataVector;
        for (int i = 0; i < w; i++) {
            chan->cwrite("newchannel"); //Used for sending strings to server, other commands: data <data>
            string s = chan->cread (); //cread gets the response
            workerChannels.push_back(new RequestChannel(s, RequestChannel::CLIENT_SIDE));
            threadIDs.push_back(i);
            workerData* wData = new workerData(workerChannels.at(i),&request_buffer,&hist);
            workerDataVector.push_back(wData);
            pthread_create(&threadIDs.at(i), NULL, worker_thread_function,wData); //Last args is null atm.
        }
        cout << "Finished making workerthreads\n";

        //Join the buffer pushing threads
        pthread_join(johnThread, NULL);
        pthread_join(janeThread, NULL);
        pthread_join(joeThread, NULL);
        cout << "Joined the request (buffer pushing) threads\n";

        delete john;
        delete jane;
        delete joe;

        //Adding all the quits to the end of the buffer. Only added after all pushing threads are done pushing
        for (int i = 0; i < w; i++) {
            request_buffer.push("quit");;
        }
        cout << "Pushed quit commands to request buffer\n";

        cout << "Closing worker threads as they finish\n";
        for (int i = 0; i < workerChannels.size(); i++) {
            pthread_join(threadIDs.at(i), NULL);
            delete workerChannels.at(i);
            delete workerDataVector.at(i);
        }
        gettimeofday(&end, NULL); //Timing end

        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl;
        //Print time spent im microseconds
        printf("%ld\n", ((end.tv_sec * 1000000 + end.tv_usec)
                         - (start.tv_sec * 1000000 + start.tv_usec)));

        hist.print ();
    }
}
