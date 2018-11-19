//
// Created by toni on 11/13/18.
//

#include "requestchannel.h"


void EXITONERROR (string msg){
    perror (msg.c_str());
    exit (-1);
}

RequestChannel::~RequestChannel() {}

RequestChannel::RequestChannel( const string _name , const Side _side) {}

RequestChannel::RequestChannel(){};


