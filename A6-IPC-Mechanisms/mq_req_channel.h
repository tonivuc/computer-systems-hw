//
// Created by toni on 11/16/18.
//

#ifndef A6_IPC_MECHANISMS_MQ_REQ_CHANNEL_H
#define A6_IPC_MECHANISMS_MQ_REQ_CHANNEL_H

#import "requestchannel.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <vector>
#include <unistd.h>

#define MSGMAX 200// default: 8192 //I feel like this should have been defined already, but this is the default anyway

using namespace std;

struct my_msgbuf {
    long mtype;
    char mtext[200];
};


class MQRequestChannel : public RequestChannel {

private:
    struct my_msgbuf buf;
    int clientToServerMqId;
    int serverToClientMqId;
    Side     my_side;
    int writeMqId;
    int readMqId;
    string my_name;
    //vector<ofstream> files;
    vector<string> filenames;


public:

    MQRequestChannel ( const string _name , const Side _side);
    MQRequestChannel();

    string cread();
    /* Blocking read of data from the channel. Returns a string of
    characters read from the channel. Returns NULL if read failed. */

    int cwrite(string msg);

    int createQueue(string mqFileName, Mode mode);
    string mq_name(Mode _mode);

    int getReadMQId();
    int getWriteMQId();
    string getServerOrClient();

    ~MQRequestChannel();
    /* Write the data to the channel. The function returns
    the number of characters written to the channel. */
};

#endif //A6_IPC_MECHANISMS_MQ_REQ_CHANNEL_H
