//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "shm_req_channel.h"

SHMRequestChannel::SHMRequestChannel(const std::string _name, const Side _side) {
    key_t key;
    int shmid;
    string filename = _name;

    std::ofstream file {filename.c_str()}; //Use a vector to delete these later?
    filenames.push_back(filename);

    /* make the key: */
    if ((key = ftok(filename.c_str(), 200)) == -1) {
        perror("ftok");
        exit(1);
    }

    /* Create the segment: */
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    /* attach to the segment to get a pointer to it: */
    data = (char*)shmat(shmid, 0, 0);
    if (*data == -1) {
        perror("shmat");
        exit(1);
    }
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
