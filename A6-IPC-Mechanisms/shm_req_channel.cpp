//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "shm_req_channel.h"

SHMRequestChannel::SHMRequestChannel(const std::string _name, const Side _side) {

    my_side = _side;
    cout << "In SHMRequestChannel on "<<getServerOrClient()<<" bro! With filename "<<_name<<endl;

    string filename = _name;
    string filename2 = _name+"_2";

    //Make two SHMBoundedBuffer objects
    bb1 = new SHMBoundedBuffer(filename);
    bb2 = new SHMBoundedBuffer(filename2);
}


int SHMRequestChannel::cwrite(string msg) {
    cout << getServerOrClient()<<" writing "<<msg<<endl;
    if (my_side == SERVER_SIDE) {
        //cout << "bb1 push Writing "<<msg<<" to client from server" <<endl;
        bb1->push(msg);
        cout << getServerOrClient() << " Pushed "<<msg<<" via shmID: "<<getServerWriteMemSegId()<<endl;
        //cout << "DONE bb1 push "<<msg<<" to client from server" <<endl;
    }
    else {
        bb2->push(msg);
        cout << getServerOrClient() << " Pushed "<<msg<<" via shmID: "<<getServerReadMemSegId()<<endl;
    }
    return 0;
}

string SHMRequestChannel::cread() {
    cout << getServerOrClient()<<" about to read "<<endl;
    if (my_side == SERVER_SIDE) {
        //cout << "Right before bb2 pop (on server)"<<endl;
        string temp = bb2->pop();
        cout << getServerOrClient()<<" just popped "<<temp<<" from "<<getServerReadMemSegId()<<endl;
        return temp;
    }
    else {
        //cout << "Right before bb1 pop (on client)"<<endl;
        string temp = bb1->pop();
        cout << getServerOrClient()<<" just popped "<<temp<<" from "<<getServerWriteMemSegId()<<endl;
        return temp;
    }
}

SHMRequestChannel::~SHMRequestChannel() {
    cout << "In delete part of code ayy!"<<endl;
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
