//
// Created by toni on 11/29/18.
//

#ifndef A7_NETWORKED_DATASERVER_NETWORK_REQ_CHANNEL_H
#define A7_NETWORKED_DATASERVER_NETWORK_REQ_CHANNEL_H


//
// Created by toni on 11/16/18.
//

#ifndef A6_IPC_MECHANISMS_NetworkRequestChannel_H
#define A6_IPC_MECHANISMS_NetworkRequestChannel_H
#import "requestchannel.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


class NetworkRequestChannel : public RequestChannel {

private:
    Side     my_side;
    //char *data;
    int sockfd, new_fd;


public:

    NetworkRequestChannel ( const string host_name, int port, const Side _side);
    NetworkRequestChannel();

    string cread();
    /* Blocking read of data from the channel. Returns a string of
    characters read from the channel. Returns NULL if read failed. */

    int cwrite(string msg);

    string getServerOrClient();

    ~NetworkRequestChannel();
    /* Write the data to the channel. The function returns
    the number of characters written to the channel. */
};


#endif //A6_IPC_MECHANISMS_NetworkRequestChannel_H



#endif //A7_NETWORKED_DATASERVER_NETWORK_REQ_CHANNEL_H
