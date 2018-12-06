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
#include "network_req_channel.h"
#include <pthread.h>
#include <atomic>
#include <vector>

using namespace std;

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

    if (_request.compare(0, 5, "hello") == 0) {
		_channel->cwrite("hello to you too", socket_fd);
	}
	else if (_request.compare(0, 4, "data") == 0) {
		usleep(1000 + (rand() % 5000));
		string toWrite = to_string(rand() % 100);
		_channel->cwrite(toWrite,socket_fd);
	}
    else {
		_channel->cwrite("unknown request", socket_fd);
	}
}

void* receive_request_loop (void* arg) {

	dataForThread* data = (dataForThread*)arg;
	int new_fd = *(data->filedescriptor);

	//NetworkRequestChannel* channel = (NetworkRequestChannel*) _channel; //Control channel
	for(;;) { ;
		string request = data->requestChannel->cread(new_fd);

        if (request.compare("quit") == 0) {
            //cout << "--- SERVER RECEIVED QUIT ---"<<endl;
            close(new_fd);
            break;                  // break out of the loop;
        }
        process_request(data->requestChannel, request, new_fd);
	}
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

    char input;
    if (argv[0] != NULL) {

        string portNumString = argv[4];
        char* portnumber = (char*)portNumString.c_str();

        sleep(5);

        //Network socket code
        NetworkRequestChannel control_channel("localhost", portnumber, RequestChannel::SERVER_SIDE);
        cout << "Created server control_channel"<<endl;
        do {
            int new_fd = control_channel.acceptWrap();

            //Adding new_fd to heap so I can pass the pointer to receive_request_loop.
            int* fd_ptr = new int;
            *fd_ptr = new_fd;
            allFileDescriptors.push_back(fd_ptr);

            dataForThread* req_t_data = new dataForThread(&control_channel,fd_ptr);
            req_thread_data.push_back(req_t_data);

            pthread_t thread_id;
            if (pthread_create(& thread_id, NULL, receive_request_loop, req_t_data) < 0 ) {
                EXITONERROR ("thread creation error in dataserver");
            }
            pthread_detach(thread_id);
            allThreadIDs.push_back(thread_id);
        } while (1); //Server runs forever apparently

    }
    else {
        cout << "ERROR creating dataserver";
        exit(0);
    }


    cout << "Done with server while-loop"<<endl;

    /*

     */

    //Cleanup of dataserver
	cout << "***Cleaning up main\n";
	for (int i = 0; i < allThreadIDs.size(); i++) {
		delete allFileDescriptors.at(i);
		delete req_thread_data.at(i);
	}
	cout << "***Worker threads closed\n";

    cout << "Quitting server after finishing main"<<endl;
}

