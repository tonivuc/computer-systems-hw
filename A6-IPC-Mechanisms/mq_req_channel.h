//
// Created by toni on 11/16/18.
//

#ifndef A6_IPC_MECHANISMS_MQ_REQ_CHANNEL_H
#define A6_IPC_MECHANISMS_MQ_REQ_CHANNEL_H

#import "requestchannel.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MSGMAX 8192 //I feel like this should have been defined already, but this is the default anyway

using namespace std;

void EXITONERROR (string msg);

struct my_msgbuf {
    long mtype;
    char mtext[200];
};


class MQRequestChannel : public RequestChannel {

private:
    struct my_msgbuf buf;
    int msgqID;

public:

    MQRequestChannel ( const string _name , const Side _side);
    MQRequestChannel();

    string cread() = 0;
    /* Blocking read of data from the channel. Returns a string of
    characters read from the channel. Returns NULL if read failed. */

    int cwrite(string msg) = 0;

    ~MQRequestChannel();
    /* Write the data to the channel. The function returns
    the number of characters written to the channel. */
};

#endif //A6_IPC_MECHANISMS_MQ_REQ_CHANNEL_H
