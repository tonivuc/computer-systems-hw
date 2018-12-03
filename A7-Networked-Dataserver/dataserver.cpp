#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "fifo_req_channel.h"
#include "mq_req_channel.h"
#include "shm_req_channel.h"
#include "network_req_channel.h"
#include <pthread.h>
#include <atomic>
using namespace std;

std::atomic<int> numThreads(0);

pthread_mutex_t newchannel_lock;
void* receive_request_loop (void* _channel);

bool finished = false;

struct dataForThread {    /* Used as argument to thread_start() */
	NetworkRequestChannel* requestChannel;
	int* filedescriptor;

	//Constructor
	dataForThread(NetworkRequestChannel* requestChannelInp, int* filedescriptorInp) :
			requestChannel(requestChannelInp), filedescriptor(filedescriptorInp) {}
};


void process_request(RequestChannel* _channel, string _request, int socket_fd) {
    cout << "Server: Inside process_request, with request "<<_request<<endl;
    if (_request.compare(0, 5, "hello") == 0) {
		_channel->cwrite("hello to you too", socket_fd);
	}
	else if (_request.compare(0, 4, "data") == 0) {
		usleep(1000 + (rand() % 5000));
		string toWrite = to_string(rand() % 100);
		cout << "process_request writing: "<<toWrite<< " via socket_fd "<<socket_fd<<endl;
		_channel->cwrite(toWrite,socket_fd);
	}
    else {
		_channel->cwrite("unknown request", socket_fd);
	}
}

void* receive_request_loop (void* arg) {
    cout << "Server: Created a new thread"<<endl;

	dataForThread* data = (dataForThread*)arg;
	cout << "Made dataForThread object"<<endl;
	int new_fd = *(data->filedescriptor);
	cout << "new_fd = "<<new_fd<<endl;

	//NetworkRequestChannel* channel = (NetworkRequestChannel*) _channel; //Control channel
	for(;;) {
	    cout << "Inside for-loop"<<endl;

		string request = data->requestChannel->cread(new_fd);

        cout << "Server received "<<request<<" from client"<<endl;
        if (request.compare("quit") == 0) {
            cout << "--- SERVER RECEIVED QUIT ---"<<endl;
            numThreads--;
            break;                  // break out of the loop;
        }
        process_request(data->requestChannel, request, new_fd);
	}
	cout << "Broke out of receive_request_loop for-loop"<<endl;
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/


int main(int argc, char * argv[]) {
    cout << "\n#Server: Started server!!"<<endl;
	newchannel_lock = PTHREAD_MUTEX_INITIALIZER;

	vector<int*> allFileDescriptors;
	vector<int> allThreadIDs;
	vector<dataForThread*> req_thread_data;

    //Network socket code
    NetworkRequestChannel control_channel("localhost", "8080", RequestChannel::SERVER_SIDE);
    cout << "Created server control_channel"<<endl;
    do {
        cout << "Server waiting for new connection"<<endl;

        int new_fd = control_channel.acceptWrap();
        printf("server: got connection\n");

        //Adding new_fd to heap so I can pass the pointer to receive_request_loop.
        int* fd_ptr = new int;
        *fd_ptr = new_fd;
        allFileDescriptors.push_back(fd_ptr);

        dataForThread* req_t_data = new dataForThread(&control_channel,fd_ptr);
        cout << "Testing dataForThread content. FD: "<<*(req_t_data->filedescriptor)<<endl;
        req_thread_data.push_back(req_t_data);

        pthread_t thread_id;
        if (pthread_create(& thread_id, NULL, receive_request_loop, req_t_data) < 0 ) {
            EXITONERROR ("thread creation error in dataserver");
        }
        pthread_detach(thread_id);
        numThreads++;
        cout << "Numthreads: "<<numThreads<<endl;
        allThreadIDs.push_back(thread_id);
        cout << "Dataserver: Finished one while-loop!"<<endl;
        //receive_request_loop (&control_channel);
    } while (1); //Server runs forever apparently

    cout << "Done with server while-loop"<<endl;

    /*
    char input;
    if (argv[0] != NULL) {
        cout << "argv[0] "<<argv[0]<<endl;
        input = *argv[0];



    }
    else {
        cout << "ERROR creating dataserver";
        exit(0);
    }
     */

    //Cleanup of dataserver
	cout << "***Joining request handlers as they finish\n";
	for (int i = 0; i < allThreadIDs.size(); i++) {
	    cout << "Joining thread "<<i<<endl;
	    cout << "thread ID "<<allThreadIDs.at(i)<<endl;
		pthread_join(allThreadIDs.at(i), NULL);
		delete allFileDescriptors.at(i);
		delete req_thread_data.at(i);
	}
	cout << "***Worker threads closed\n";

    cout << "Quitting server after finishing main"<<endl;
}

