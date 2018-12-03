//
// Created by toni on 11/13/18.
//

#ifndef A6_IPC_MECHANISMS_REQUESTCHANNEL_H
#define A6_IPC_MECHANISMS_REQUESTCHANNEL_H

#include <iostream>
#include <fstream>
#include <exception>
#include <string>
using namespace std;

void EXITONERROR (string msg);

class RequestChannel {
public:
    typedef enum {SERVER_SIDE, CLIENT_SIDE} Side ;
    typedef enum {READ_MODE, WRITE_MODE} Mode ;

    /* CONSTRUCTOR/DESTRUCTOR */
    RequestChannel (const string host_name, char* port, const Side _side);
    RequestChannel();

    virtual string cread(int sockfd) = 0;
    /* Blocking read of data from the channel. Returns a string of
    characters read from the channel. Returns NULL if read failed. */

    virtual int cwrite(string msg, int socketf) = 0;

    virtual ~RequestChannel();
    /* Write the data to the channel. The function returns
    the number of characters written to the channel. */
};

#endif //A6_IPC_MECHANISMS_REQUESTCHANNEL_H
