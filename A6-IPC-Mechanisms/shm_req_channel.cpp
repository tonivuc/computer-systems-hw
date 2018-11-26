//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "shm_req_channel.h"

SHMRequestChannel::SHMRequestChannel(const std::string _name, const Side _side) {

    cout << "In SHMRequestChannel on "<<getServerOrClient()<<" bro!"<<endl;
    my_side = _side;

    string filename = _name;
    string filename2 = _name+"_2";

    //Make two SHMBoundedBuffer objects
    bb1 = new SHMBoundedBuffer(filename);
    bb2 = new SHMBoundedBuffer(filename2);
}


int SHMRequestChannel::cwrite(string msg) {
    cout << getServerOrClient()<<" writing "<<msg<<endl;
    if (my_side == SERVER_SIDE) {
        bb1->push(msg);
    }
    else {
        bb2->push(msg);
    }
}

string SHMRequestChannel::cread() {
    cout << getServerOrClient()<<" about to read "<<endl;
    if (my_side == SERVER_SIDE) {
        return bb2->pop();
    }
    else {
        return bb1->pop();
    }
}

SHMRequestChannel::~SHMRequestChannel() {
    cout << "In delete part of code ayy!"<<endl;
    delete bb1;
    delete bb2;
}

string SHMRequestChannel::getServerOrClient() {
    return (my_side == SHMRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
}
