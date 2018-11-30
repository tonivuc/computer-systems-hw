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
using namespace std;


int nchannels = 0;
pthread_mutex_t newchannel_lock;
void* handle_process_loop (void* _channel);

void process_newchannel(RequestChannel* _channel, char mqType) {
	nchannels ++;
	string new_channel_name = "data" + to_string(nchannels);
	_channel->cwrite(new_channel_name); //Writing to control channel. AAAH, so MAIN can get it back.
    NetworkRequestChannel* data_channel = new NetworkRequestChannel(localhost, hostport, RequestChannel::SERVER_SIDE);

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
    else if (_request.compare("newchannel") == 0) { //Default
        process_newchannel(_channel, 'f');
    }
    else {
		_channel->cwrite("unknown request");
	}
}

void* handle_process_loop (void* _channel, int new_fd) {
	char buf [1024];

	NetworkRequestChannel* channel = (NetworkRequestChannel*) _channel; //Control channel
	for(;;) { ;
		channel->cread();
		recv (new_fd, buf, sizeof (buf), 0);
		//string request = channel->cread();
		if (request.compare("quit") == 0) {
		    //cout << "--- SERVER RECEIVED QUIT ---"<<endl;
			break;                  // break out of the loop;
		}
		printf("server: received msg: %s\n", buf);
		process_request(channel, request);
	}
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/


int main(int argc, char * argv[]) {
    cout << "\n#Server: Started server!!"<<endl;
	newchannel_lock = PTHREAD_MUTEX_INITIALIZER;

    char input;
    if (argv[0] != NULL) {
        cout << "argv[0] "<<argv[0]<<endl;
        input = *argv[0];

        //Network socket code
        NetworkRequestChannel control_channel(localhost, hostport, RequestChannel::SERVER_SIDE);
        while (1) {
			int new_fd = control_channel.accept();
			printf("server: got connection\n");
			handle_process_loop (&control_channel);
        }

    }
    else {
        cout << "ERROR creating dataserver";
        exit(0);
    }
    cout << "Quitting server after finishing main"<<endl;
}

