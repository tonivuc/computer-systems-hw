//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "shm_req_channel.h"

SHMRequestChannel::SHMRequestChannel(const std::string _name, const Side _side) {

    my_side = _side;
    cout << "SHMReqChn: "<<getServerOrClient()<<" bro! With filename "<<_name<<endl;

    string filename = _name;
    string filename2 = _name+"_2";

    //Make two SHMBoundedBuffer objects
    bb1 = new SHMBoundedBuffer(filename);
    bb2 = new SHMBoundedBuffer(filename2);
}


int SHMRequestChannel::cwrite(string msg) {
    if (my_side == SERVER_SIDE) {
        bb1->push(msg);
    }
    else {
        bb2->push(msg);
    }
    return 0;
}

string SHMRequestChannel::cread() {
    if (my_side == SERVER_SIDE) {
        string temp = bb2->pop();
        return temp;
    }
    else {
        //cout << "Right before bb1 pop (on client)"<<endl;
        string temp = bb1->pop();
        return temp;
    }
}

SHMRequestChannel::~SHMRequestChannel() {
    delete bb1;
    delete bb2;
}

string SHMRequestChannel::getServerOrClient() {
    if (my_side == SHMRequestChannel::SERVER_SIDE) {
        return "SERVER";
    }
    else if (my_side == SHMRequestChannel::CLIENT_SIDE) {
        return "CLIENT";
    }
    else {
        return "getServerOrClient ERROR";
    }
}

int SHMRequestChannel::getServerWriteMemSegId() {
    return bb1->getShmid();
}

int SHMRequestChannel::getServerReadMemSegId() {
    return bb2->getShmid();
}
