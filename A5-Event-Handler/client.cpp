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

#include "reqchannel.h"
#include "BoundedBuffer.h"
#include "Histogram.h"

#ifdef __cplusplus__
#include <cstdlib>
#else
#include <stdlib.h>
#endif

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
    RequestChannel *work_channel;
    BoundedBuffer *req_buffer; //Need to know where to push
    BoundedBuffer *responseBuffer[3];

    //Constructor
    workerData(RequestChannel *work_channel_inp, BoundedBuffer *req_buffer_inp, BoundedBuffer *responseBufferInp[3]) :
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
        //cout << "pushing data "<<i<<" "<<data->data_string<<" to requestBuffer"<<endl;
    };
    //Retval is used by the pthread_join() function
    return NULL; //For now
}

void* worker_thread_function(void* arg) {

    workerData* data = (workerData*)arg;


    bool run = true;

    while(run) {
        string request = data->req_buffer->pop(); //Already has mutex in the buffer
        data->work_channel->cwrite(request); //Sends "requests" to the server
        //Add to that string thing

        if(request == "quit") {
            run = false;
        }
        /*
        else {
            string response = data->work_channel->cread();
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
            //Need to put request + response in the histogram buffer. Struct?
            //data->hist->update(request, response); No longer allowed. Histogram threads do this now
        }
         */ //Handling of responses moved somewhere else
    }
    return NULL;
}

void* stat_thread_function(void* arg) {

    histogramData* data = (histogramData*)arg;
    cout << "Before for-loop in stat_thread_function"<<endl;
    for(int i = 0; i < data->n; i++) {
        //cout << "Popping from end buffer"<<endl;
        string response = data->response_buffer->pop();
        //cout << "Popping from end buffer data: "<<response<<endl;
        //cout << "n = "<<i<<" Popped "<<response<<" from responsebuffer "<<data->data_name<<endl;
        data->hist->update(data->data_name,response);
    }
    cout << "Finished stat_thread!"<<endl;
    return NULL;
}


//Returns pointer to vector of data channels?
int create_data_channels(RequestChannel &controlChannel, vector<RequestChannel*> &dataChannels, int w, fd_set &readfds, vector<int> &fileDescriptors) {

    //These have to be empty for indexing to work out later.
    dataChannels.clear();
    fileDescriptors.clear();

    RequestChannel* dataChannel;
    for (int i = 0; i < w; i++) {
        controlChannel.cwrite("newchannel"); //Used for sending strings to server, other commands: data <data>
        string s = controlChannel.cread (); //cread gets the response
        dataChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
        int readFD = dataChannel->read_fd();
        FD_SET(readFD, &readfds); //Add the file descriptor used for reading from the channel, to the set.
        fileDescriptors.push_back(readFD); //Only pushing the read file descriptors. So size is the same as the request channel size
        dataChannels.push_back(dataChannel);
    }
    return dataChannel->read_fd(); //Returns highest file descriptor
}


void sendInitialData(vector<string> currentRequestChannelData, vector<RequestChannel*> &dataChannels, int &k) {

}

void handle_data_channels(RequestChannel &controlChannel, vector<RequestChannel*> &dataChannels, BoundedBuffer &requestBuffer, BoundedBuffer responseBuffers[3], int w, int n) {
    //Local variables
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 500000;
    fd_set readfds; //A set containing all the file descriptors that are ready for reading
    vector<int> allReadFDVector;
    vector<string> currentRequestChannelData;
    FD_ZERO(&readfds); //Clear the set before we begin (might not need this?)
    bool loop = true;
    int k = 0; //How many data pieces we have pushed to server so far


    //Create the data channels (without pushing anything to them)
    int maxfds = create_data_channels(controlChannel, dataChannels, w, readfds, allReadFDVector);
    fd_set allReadFDSet = readfds;

    //Send data to all channels
    for (int i = 0; i < w; i++) {
        string request = requestBuffer.pop(); //Already has mutex in the buffer
        currentRequestChannelData.push_back(request);
        dataChannels.at(i)->cwrite(request); //Sends "requests" to the serve;
        cout << "Pushing "<<request<<" to server."<<endl;
        k++;
    }
    bool debug = false;

    int one = 0, two = 0, three = 0;

    cout << "allReadFDVector.size() "<<allReadFDVector.size()<<endl;

    // don't care about writefds and exceptfds:
    while (loop) {
        readfds = allReadFDSet; //Reset the set before a new selection
        select(maxfds+1, &readfds, NULL, NULL, NULL);
        //After running select, the readfds set only contains the file descriptors that are ready

        //Call RequestChannel::cread() on the channels that are ready and write new data to available channels
        for (int i = 0; i < allReadFDVector.size(); i++) {

            if FD_ISSET(allReadFDVector.at(i), &readfds) { //If this fd has been changed

                string response = dataChannels.at(i)->cread(); // i = the ith channel
                string request = currentRequestChannelData.at(i); //i lets us know which datachannel we are looking at
                //cout << "Reading from channel w. request"<<request<<", w. response "<<response<<endl;

                if (request.compare("data John Smith") == 0) {

                    responseBuffers[0].push(response); //ResponseBuffer already has built-in mutex
                    cout << "size of responeBuffer1 is "<<responseBuffers[0].size()<<endl;
                    one++;
                    if (debug) cout << "Pushing "<<i<<" "<<request<<" to responsebuffer 0."<<endl;
                }
                else if (request.compare("data Jane Smith") == 0) {

                    responseBuffers[1].push(response);
                    cout << "size of responeBuffer2 is "<<responseBuffers[1].size()<<endl;
                    two++;
                    if (debug) cout << "Pushing "<<i<<" "<<request<<" to responsebuffer 1."<<endl;
                }
                else if (request.compare("data Joe Smith") == 0) {
                    three++;

                    responseBuffers[2].push(response);
                    cout << "size of responeBuffer3 is "<<responseBuffers[2].size()<<endl;
                    if (debug) cout << "Pushing "<<i<<" "<<request<<" to responsebuffer 2."<<endl;
                }
                else {
                    cout<<"ERROR ERROR ERROR ERROR in WorkerThread. Request data is not correct!\n";
                }
                //cout << "k: "<<k<<" n: "<<n<<endl;
                //Send more data to the server
                if (k < 3*n) {
                    string request = requestBuffer.pop();
                    currentRequestChannelData.at(i) = request;
                    dataChannels.at(i)->cwrite(request); //Sends "requests" to the server
                    k++;
                }
                else if ((one + two + three) == n*3){
                    cout << "Loop = false"<<endl;
                    loop = false;
                    //Loop through the channels and send quit
                }
                else {
                    cout<<"Waiting for all responses from server"<<endl;
                }
            }
        }
    }
    cout <<endl;
    cout << "one "<<one<<" two "<<two<<" three "<<three<<endl;


    for (int i = 0; i < w; i++) {
        //cout << "Pushing quit to server"<<endl;
        //dataChannels.at(i)->cwrite("quit");
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

    //signal(SIGALRM, sigalarm_handler); //Map SIGALARM to the sigalarm_handler function
    //alarm(1); //Start sigalarm_handler after 2 seconds

    //New thread in main
    struct timeval start, end;

    int n = 10000; //default number of requests per "patient"
    int w = 100; //Default number of data channels
    int b = 6;
    int opt = 0;

    vector<string> data = {"data John Smith","data Jane Smith","data Joe Smith"};

    while ((opt = getopt(argc, argv, "n:w:b:")) != -1) {
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
        }
    }

    int pid = fork();
    if (pid == 0){
        execl("dataserver", (char*) NULL);
    }
    else {

        cout << "n == " << n << endl;
        cout << "w == " << w << endl;
        cout << "b == " << b << endl;

        cout << "CLIENT STARTED:" << endl;
        cout << "Establishing control channel... " << flush; //What is this? endl. Forces it to be printed immediately.
        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE); //Have to specify client side so it knows how the object will work
        cout << "done." << endl<< flush;

        //Timing:
        gettimeofday(&start, NULL);

        //Making BoundedBuffers
        BoundedBuffer request_buffer(b);

        //Keep the response buffers > 0.
        if (b < 3) {
            b = 3;
            printf("Changed buffer size so responseBuffers have size 1");
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
        vector<RequestChannel*> workerChannels; //dataChannel
        for (int i = 0; i < w; i++) {
            chan->cwrite("newchannel"); //Used for sending strings to server, other commands: data <data>
            string s = chan->cread (); //cread gets the response
            workerChannels.push_back(new RequestChannel(s, RequestChannel::CLIENT_SIDE));
        }
        cout << "***Finished making workerchannels (actually datachannels)\n";
        //pushData(n, &request_buffer);

        //////////////////////////////////

        // New code goes here

        handle_data_channels(*chan, workerChannels,request_buffer,*responseBuffers,w,n);

        /////////////////////////////////

        //Join the buffer pushing threads
        cout << "***Joining requestBuffer pushing threads";
        for (int i = 0; i < data.size(); i++) {
            pthread_join(req_thread_IDs.at(i), NULL);
            delete req_thread_data.at(i); //Clear this from heap
        }
        cout << "***Joined the request (buffer pushing) threads\n";


        //Adding all the quits to the end of the buffer. Only added after all pushing threads are done pushing
        /*
        for (int i = 0; i < w; i++) {
            request_buffer.push("quit");;
        }
        cout << "***Pushed quit commands to request buffer\n";
         */

        cout << "***Deleting data/worker-channels\n";
        for (int i = 0; i < workerChannels.size(); i++) {
            delete workerChannels.at(i);
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

        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl;
        //Print time spent im microseconds
        printf("%ld\n", ((end.tv_sec * 1000000 + end.tv_usec)
                         - (start.tv_sec * 1000000 + start.tv_usec)));
        system("clear");
        hist.print ();
        printf("Runtime: %ld microseconds\n", ((end.tv_sec * 1000000 + end.tv_usec)
                         - (start.tv_sec * 1000000 + start.tv_usec)));
    }
}
