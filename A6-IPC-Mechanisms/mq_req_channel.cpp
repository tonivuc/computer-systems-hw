//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "mq_req_channel.h"

void EXITONERROR (string msg){
    perror (msg.c_str());
    exit (-1);
}


MQRequestChannel::MQRequestChannel(const std::string _name, const Side _side) {

    if (_side == SERVER_SIDE) {

    }
    else {

    }
}

MQRequestChannel::CreateQueue() {
    //buf.mtype = 1;
    key_t key = ftok ("a.txt", 100); // create a pseudo-random key
    if (key == -1) {
        perror("Problem getting key");
    }
    int msqid = msgget(key, 0644 | IPC_CREAT); // you want to connect to a queue, or create it if it doesn't exist
    if (msqid < 0){
        perror ("Message Queue could not be created");
        return 0;
    }
    printf ("Message Queued ID: %ld\n", msqid);

}

int MQRequestChannel::cwrite(string msg) {

    /*
    while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
        int len = strlen(buf.mtext);
        if (msgsnd(msqid, &buf, len+1, 0) == -1)
            perror ("msgsnd");
    }
     */
    int len = msg.length();
    if (msg.size() > MSGMAX) {
        EXITONERROR ("cwrite");
        return -1;
    }
    if (msgsnd(msgqID, &msg, len+1, 0) == -1) {
        EXITONERROR ("cwrite");
        return -1;
    }
    return 0;
}

string MQRequestChannel::cread() {

    char buf [MAX_MESSAGE];
    if (read(rfd, buf, MAX_MESSAGE) <= 0) {
        EXITONERROR ("cread");
    }
    string s = buf;
    return s;

}



MQRequestChannel::~MQRequestChannel() {
    close(wfd);
    close(rfd);
    //if (my_side == SERVER_SIDE) {
    remove(pipe_name(READ_MODE).c_str());
    remove(pipe_name(WRITE_MODE).c_str());
    //}
}
