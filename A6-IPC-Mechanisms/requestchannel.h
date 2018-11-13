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

class RequestChannel {
public:
    typedef enum {SERVER_SIDE, CLIENT_SIDE} Side ;
    typedef enum {READ_MODE, WRITE_MODE} Mode ;

    /* CONSTRUCTOR/DESTRUCTOR */
    RequestChannel ( const string _name , const Side _side) ;
    RequestChannel() {};

    ~RequestChannel ( );

    virtual string cread() = 0;
    /* Blocking read of data from the channel. Returns a string of
    characters read from the channel. Returns NULL if read failed. */

    virtual int cwrite(string msg) = 0;
    /* Write the data to the channel. The function returns
    the number of characters written to the channel. */
};

#endif //A6_IPC_MECHANISMS_REQUESTCHANNEL_H
