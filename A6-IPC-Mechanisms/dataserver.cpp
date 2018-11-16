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
#include <pthread.h>
using namespace std;


int nchannels = 0;
pthread_mutex_t newchannel_lock;
void* handle_process_loop (void* _channel);

void process_newchannel(FIFORequestChannel* _channel) {
	nchannels ++;
	string new_channel_name = "data" + to_string(nchannels) + "_";
	_channel->cwrite(new_channel_name);
	FIFORequestChannel * data_channel = new FIFORequestChannel(new_channel_name, FIFORequestChannel::SERVER_SIDE);
	pthread_t thread_id;
	if (pthread_create(& thread_id, NULL, handle_process_loop, data_channel) < 0 ) {
		EXITONERROR ("");
	}

}

void process_request(FIFORequestChannel* _channel, string _request) {

	if (_request.compare(0, 5, "hello") == 0) {
		_channel->cwrite("hello to you too");
	}
	else if (_request.compare(0, 4, "data") == 0) {
		usleep(1000 + (rand() % 5000));
		_channel->cwrite(to_string(rand() % 100));
	}
	else if (_request.compare(0, 10, "newchannel") == 0) {
		process_newchannel(_channel);
	}
	else {
		_channel->cwrite("unknown request");
	}
}

void* handle_process_loop (void* _channel) {
	FIFORequestChannel* channel = (FIFORequestChannel *) _channel;
	for(;;) {
		string request = channel->cread();
		if (request.compare("quit") == 0) {
			break;                  // break out of the loop;
		}
		process_request(channel, request);
	}
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/


int main(int argc, char * argv[]) {
	newchannel_lock = PTHREAD_MUTEX_INITIALIZER;
	FIFORequestChannel control_channel("control", FIFORequestChannel::SERVER_SIDE);
	handle_process_loop (&control_channel);	
}

