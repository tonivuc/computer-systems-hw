//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "shm_req_channel.h"

SHMRequestChannel::SHMRequestChannel(const std::string _name, const Side _side) {

    //Make two SHMBoundedBuffer objects

    string filename = _name;






}


int SHMRequestChannel::cwrite(string msg) {
    /* read or modify the segment, based on the command line: */
    printf("writing to segment: \"%s\"\n", msg.c_str());
    strncpy(data, msg.c_str(), SHM_SIZE);
    return 0;
}

string SHMRequestChannel::cread() {

    /* read from the shared-memory segment: */
    printf("***********segment contains: \"%s\"\n", data);

    string s = data;
    return s;
}



SHMRequestChannel::~SHMRequestChannel() {
    /* detach from the segment: */
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }
    //Remove files created
    for (int i = 0; i < filenames.size(); i++) {
        remove(filenames.at(i).c_str());
    }
}

string SHMRequestChannel::getServerOrClient() {
    return (my_side == SHMRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
}
