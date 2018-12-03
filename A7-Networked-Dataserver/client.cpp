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

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <bits/signum.h>

#include "fifo_req_channel.h"
#include "mq_req_channel.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "shm_req_channel.h"
#include "network_req_channel.h"

#ifdef __cplusplus__
#include <cstdlib>
#else
#include <stdlib.h>
#endif

//127.0.0.1

using namespace std;
Histogram hist;

struct dataForThread {    /* Used as argument to thread_start() */
    int     n;
    string   data_string;
    BoundedBuffer *req_buffer; //Need to know where to push

    //Constructor
    dataForThread(int nInp, string data_string_inp, BoundedBuffer *req_buffer_inp) :
            n(nInp), data_string(data_string_inp), req_buffer(req_buffer_inp) {}
};

struct workerData {    /* Used as argument to thread_start() */
    NetworkRequestChannel *work_channel;
    BoundedBuffer *req_buffer; //Need to know where to push
    BoundedBuffer *responseBuffer[3];

    //Constructor
    workerData(NetworkRequestChannel *work_channel_inp, BoundedBuffer *req_buffer_inp, BoundedBuffer *responseBufferInp[3]) :
            work_channel(work_channel_inp), req_buffer(req_buffer_inp) {

        responseBuffer[0] = responseBufferInp[0];
        responseBuffer[1] = responseBufferInp[1];
        responseBuffer[2] = responseBufferInp[2];
    }
};


struct histogramData {    /* Used as argument to thread_start() */
    int n;
    string data_name;
    BoundedBuffer *response_buffer; //ACTUALLY an array
    Histogram *hist;

    //Constructor
    histogramData(int n_inp, BoundedBuffer *resp_buffer_inp, string data_name_inp, Histogram *hist_inp) :
            n(n_inp), response_buffer(resp_buffer_inp), data_name(data_name_inp), hist(hist_inp) {}
};

//This function is fed to the thread as "start_routine"F
void* request_thread_function(void* arg) {

    dataForThread* data = (dataForThread*)arg;

    for(int i = 0; i < data->n; i++) {
        data->req_buffer->push(data->data_string);
    };
    //Retval is used by the pthread_join() function
    return NULL; //For now
}

void* worker_thread_function(void* arg) {

    workerData* data = (workerData*)arg;

    bool run = true;

    while(run) {
        string request = data->req_buffer->pop(); //Already has mutex in the buffer
        data->work_channel->cwrite(request,data->work_channel->getMainFD()); //Sends "requests" to the server
        if(request == "quit") {
            run = false;
        }
        else {
            string response = data->work_channel->cread(data->work_channel->getMainFD());
            if (request.compare("data John Smith") == 0) {
                data->responseBuffer[0]->push(response); //ResponseBuffer already has built-in mutex
            }
            else if (request.compare("data Jane Smith") == 0) {
                data->responseBuffer[1]->push(response);
            }
            else if (request.compare("data Joe Smith") == 0) {
                data->responseBuffer[2]->push(response);
            }
            else {
                cout<<"ERROR in WorkerThread. Request data is not correct!\n";
            }
        }
    }
    return NULL;
}

void* stat_thread_function(void* arg) {

    histogramData* data = (histogramData*)arg;

    for(int i = 0; i < data->n; i++) {

        //Would need a mutex here to sync pushing and popping of response_buffer?
        string response = data->response_buffer->pop();
        data->hist->update(data->data_name,response);

    }
}

//Runs 2 seconds after alarm(2) is called
void sigalarm_handler(int input) {
    system("clear");
    hist.print();
    alarm(2);
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

    /*
    signal(SIGALRM, sigalarm_handler); //Map SIGALARM to the sigalarm_handler function
    alarm(1); //Start sigalarm_handler after 2 seconds
     */

    //New thread in main
    struct timeval start, end;

    int n = 100000; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int b = 10;
    int opt = 0;
    string hostname;
    char* hostport;

    char mqType = 0;

    vector<string> data = {"data John Smith","data Jane Smith","data Joe Smith"};

    while ((opt = getopt(argc, argv, "n:w:b:i:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function hmm
                break;
            case 'b':
                b = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function
                break;
            case 'i':
                mqType = *optarg; //This won't do a whole lot until you fill in the worker thread function
                break;
        }
    }

    hostname = "localhost";
    hostport = "8080";

    cout << "***: mqType is: "<<mqType<<endl;

    cout << "n == " << n << endl;
    cout << "w == " << w << endl;
    cout << "b == " << b << endl;

    NetworkRequestChannel *chan = new NetworkRequestChannel(hostname, hostport, RequestChannel::CLIENT_SIDE);

    cout << "Client.cpp: CLIENT STARTED:" << endl;

    //Timing:
    gettimeofday(&start, NULL);


    //Making BoundedBuffers
    BoundedBuffer request_buffer(b);

    //Keep the response buffers > 0.
    if (b < 3) {
        b = 3;
        printf("Client.cpp: Changed buffer size so responseBuffers have size 1\n");
    }

    //Store pointers to the response buffers in an array
    BoundedBuffer responseBufferJohn(b/3);
    BoundedBuffer responseBufferJane(b/3);
    BoundedBuffer responseBufferJoe(b/3);
    //Array of BoundedBuffer pointers
    BoundedBuffer *responseBuffers[3]; //Pass this to histogram-threads
    responseBuffers[0] = &responseBufferJohn;
    responseBuffers[1] = &responseBufferJane;
    responseBuffers[2] = &responseBufferJoe;


    //Create request and stat/histogram threads
    vector<pthread_t> req_thread_IDs(3,0);
    vector<pthread_t> histThreadIDs(3, 0);
    vector<dataForThread*> req_thread_data;
    vector<histogramData*> data_for_hist_threads;


    //Create 3 request threads
    for (int i = 0; i < data.size(); i++) {
        //Request threads pushing data to server starting
        dataForThread* req_t_data = new dataForThread(n,data.at(i),&request_buffer);
        req_thread_data.push_back(req_t_data);
        pthread_create(&req_thread_IDs.at(i), NULL, request_thread_function, req_thread_data.at(i)); //Create request threads

        //Histogram updating threads starting
        data_for_hist_threads.push_back(new histogramData(n, responseBuffers[i], data.at(i), &hist));
        pthread_create(&histThreadIDs.at(i), NULL, stat_thread_function,data_for_hist_threads.at(i));
    }
    cout << "***Finished making request and histogram threads\n";


    //Create worker threads and channels
    vector<NetworkRequestChannel*> workerChannels;
    vector<pthread_t> threadIDs;
    vector<workerData*> workerDataVector;

    for (int i = 0; i < w; i++) {
        //Socket code
        workerChannels.push_back(new NetworkRequestChannel(hostname, hostport, RequestChannel::CLIENT_SIDE));

        threadIDs.push_back(i);
        workerData* wData = new workerData(workerChannels.at(i),&request_buffer,responseBuffers); //FIFORequestChannel *work_channel_inp, BoundedBuffer *req_buffer_inp, BoundedBuffer *responseBufferInp[3])
        workerDataVector.push_back(wData);
        pthread_create(&threadIDs.at(i), NULL, worker_thread_function,wData); //Last args is null atm.
    }
    cout << "***Finished making workerthreads\n";
    //pushData(n, &request_buffer);


    //Join the buffer pushing threads
    for (int i = 0; i < data.size(); i++) {
        pthread_join(req_thread_IDs.at(i), NULL);
        delete req_thread_data.at(i); //Clear this from heap
    }
    cout << "***Joined the request (buffer pushing) threads\n";


    //Adding all the quits to the end of the buffer. Only added after all pushing threads are done pushing
    for (int i = 0; i < w; i++) {
        request_buffer.push("quit");;
    }
    cout << "***Pushed quit commands to request buffer\n";

    cout << "***Closing worker threads as they finish\n";
    for (int i = 0; i < workerChannels.size(); i++) {
        pthread_join(threadIDs.at(i), NULL);
        delete workerChannels.at(i);
        delete workerDataVector.at(i);
    }
    cout << "***Worker threads closed\n";

    //Close the histogram threads
    cout << "***Closing histogram threads as they finish\n";
    for (int i = 0; i < data.size(); i++) {
        pthread_join(histThreadIDs.at(i), NULL);
        delete data_for_hist_threads.at(i); //Clear this from heap
    }
    cout << "***Histogram threads closed\n";

    gettimeofday(&end, NULL); //Timing end

    chan->cwrite("quit", chan->getMainFD());
    delete chan;
    cout << "All Done!!!" << endl;
    //Print time spent im microseconds
    printf("%ld\n", ((end.tv_sec * 1000000 + end.tv_usec)
                     - (start.tv_sec * 1000000 + start.tv_usec)));
    //system("clear");
    hist.print ();
    printf("Runtime: %ld microseconds\n", ((end.tv_sec * 1000000 + end.tv_usec)
                                           - (start.tv_sec * 1000000 + start.tv_usec)));

}
