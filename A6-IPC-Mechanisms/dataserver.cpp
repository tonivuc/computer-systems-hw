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
#include <pthread.h>
using namespace std;


int nchannels = 0;
pthread_mutex_t newchannel_lock;
void* handle_process_loop (void* _channel);

void process_newchannel(RequestChannel* _channel, char mqType) {
	nchannels ++;
	string new_channel_name = "data" + to_string(nchannels);
	//cout << "###New channel name in dataserver: "<<new_channel_name<<endl;
    //cout << "###Writing to server memory segment: "<<((SHMRequestChannel*)_channel)->getServerWriteMemSegId()<<endl;
	_channel->cwrite(new_channel_name); //Writing to control channel. AAAH, so MAIN can get it back.
	cout << "#Server: wrote "<<new_channel_name<<" to client via control channel"<<endl;
	RequestChannel* data_channel;
    switch (mqType) {
        case 'f': {
            data_channel = new FIFORequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
            break;
        }
        case 'q': {
            data_channel = new MQRequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
            break;
        }
        case 's': {
            data_channel = new SHMRequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
            break;
        }
        default:
            data_channel = new FIFORequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
    }

	pthread_t thread_id;
	if (pthread_create(& thread_id, NULL, handle_process_loop, data_channel) < 0 ) {
		EXITONERROR ("");
	}

}

void process_request(RequestChannel* _channel, string _request) {
	if (_request.compare(0, 5, "hello") == 0) {
		_channel->cwrite("hello to you too");
	}
	else if (_request.compare(0, 4, "data") == 0) {
		usleep(1000 + (rand() % 5000));
		_channel->cwrite(to_string(rand() % 100));
	}
	else if (_request.compare("newchannelFIFO") == 0) {
		process_newchannel(_channel, 'f'); //Passing in control channel
	}
    else if (_request.compare("newchannelMQ") == 0) {
        process_newchannel(_channel, 'q');
    }
    else if (_request.compare("newchannelSHM") == 0) {
        process_newchannel(_channel, 's');
    }
    else if (_request.compare("newchannel") == 0) { //Default
        process_newchannel(_channel, 'f');
    }
    else {
		_channel->cwrite("unknown request");
	}
}

void* handle_process_loop (void* _channel) {

	RequestChannel* channel = (RequestChannel*) _channel; //Control channel
	for(;;) {
        cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHAHAHAHAHAHAHAHAH"<<endl;
	    //cout << "---Server reading from control channel: "<<((SHMRequestChannel*)channel).get <<endl;
        //sleep(1);
	    cout << "#Server: In server for-loop about to reads"<<endl;
		string request = channel->cread();
		cout << "#Server: request: "<<request<<" received on server"<<endl;
		if (request.compare("quit") == 0) {
		    cout << "--- SERVER RECEIVED QUIT ---"<<endl;
			break;                  // break out of the loop;
		}
		process_request(channel, request);
		cout<<endl;
	}
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/


int main(int argc, char * argv[]) {
    cout << "\n#Server: Started server!!"<<endl;
	newchannel_lock = PTHREAD_MUTEX_INITIALIZER;

    //cout << "Server argv[0] "<<argv[0]<<endl;

    char input;
    if (argv[0] != NULL) {
        //cout << "argv[0] "<<argv[0]<<endl;
        input = *argv[0];

        switch (input) {
            case 'f': {
                FIFORequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
                handle_process_loop (&control_channel); //Delete control_channel? //Control channel is passed in
                break;
            }
            case 'q': {
                MQRequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
                handle_process_loop (&control_channel); //Delete control_channel? //Control channel is passed in
                break;
            } //test
            case 's': {
                SHMRequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
                handle_process_loop (&control_channel); //Delete control_channel? //Control channel is passed in
                break;

            }
            default: {
                FIFORequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
                handle_process_loop (&control_channel);
            }
        }
    }
    else {
        cout << "ELSE in dataserver"<<endl;
        FIFORequestChannel control_channel("control", RequestChannel::SERVER_SIDE);
        handle_process_loop (&control_channel); //Delete control_channel?
    }
    cout << "Quitting server after finishing main"<<endl;
}

