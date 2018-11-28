//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "mq_req_channel.h"
#include <fstream>

MQRequestChannel::MQRequestChannel(const std::string _name, const Side _side) {
    my_name = _name;
    my_side = _side;
    string mqFileName = mq_name(WRITE_MODE);

    if (_side == SERVER_SIDE) {

        writeMqId = createQueue(mqFileName.c_str(), WRITE_MODE); //Open mq to write to
        readMqId = createQueue(mqFileName.c_str(), READ_MODE); //Open mq to read from
    }
    else {

        writeMqId = createQueue(mqFileName.c_str(), WRITE_MODE); //Open mq to read from
        readMqId = createQueue(mqFileName.c_str(), READ_MODE); //Open mq to write to
    }
}

int MQRequestChannel::createQueue(string mqFileName, Mode mode) {

    //Programs that want to use the same queue must generate the same key, so they must pass the
    //same parameters to ftok().
    key_t key;
    std::ofstream file {mqFileName.c_str()}; //Use a vector to delete these later?
    filenames.push_back(mqFileName);
    if (my_side == CLIENT_SIDE) {
        if (mode == READ_MODE)
            key = ftok(mqFileName.c_str(), 65); //Client read from 65
        else
            key = ftok(mqFileName.c_str(), 66); //Client write to 66 create a pseudo-random key. 2nd argument is usually some set number
    }
    else {
        // SERVER_SIDE
        if (mode == READ_MODE)
            key = ftok (mqFileName.c_str(), 66); //Server read from 66 create a pseudo-random key. 2nd argument is usually some set number (SERVER-->ReadMode)
        else
            key = ftok (mqFileName.c_str(), 65); //Server write to 65  create a pseudo-random key. 2nd argument is usually some set number (SERVER-->WriteMode)
    }

    if (key == -1) {
        perror("Problem getting key");
    }
    int msqid;
    if (my_side == SERVER_SIDE) {
        msqid = msgget(key, 0666 | IPC_CREAT); // you want to connect to a queue, or create it if it doesn't exist //0644
    }
    else {
        msqid = msgget(key, 0666 | IPC_CREAT); // you want to connect to a queue
    }

    if (msqid < 0){
        perror ("Message Queue could not be created");
        return 0;
    }

    return msqid;

}

std::string MQRequestChannel::mq_name(Mode _mode) {
    std::string qname = "mq_" + my_name;
    return qname;
}

int MQRequestChannel::cwrite(string msg) {

    struct my_msgbuf msgStruct;
    msgStruct.mtype = 1;

    strncpy(msgStruct.mtext, msg.c_str(), MSGMAX);

    int len = strlen(msgStruct.mtext);;
    if (msg.size() > MSGMAX) {
        EXITONERROR ("cwrite message length exceeded");
        return -1;
    }

    if (msgsnd(writeMqId, &msgStruct, len+1, 0) == -1) {
        EXITONERROR ("cwrite");
        return -1;
    }
    return 0;
}

string MQRequestChannel::cread() {

    string clientOrServer = (my_side == MQRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";

    struct my_msgbuf msgStruct;
    msgStruct.mtype = 1;

    int out = msgrcv(readMqId, &msgStruct, sizeof(msgStruct.mtext), 0, 0);
    if (out<= 0) {
        //cout << "struct content "<<msgStruct.mtext<<endl;
        EXITONERROR ("cread error");
    }

    string s = msgStruct.mtext;

    return s;
}



MQRequestChannel::~MQRequestChannel() {
    string clientOrServer = (my_side == MQRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
    //cout << "DELETING A "<< clientOrServer<< " MESSAGE QUEUE with the MQ read: "<<readMqId<< " and write:"<<writeMqId<<endl;
    msgctl(writeMqId, IPC_RMID, NULL);
    msgctl(readMqId, IPC_RMID, NULL);
    for (int i = 0; i < filenames.size(); i++) {
        remove(filenames.at(i).c_str());
    }
}

int MQRequestChannel::getReadMQId() {
    return readMqId;
}

int MQRequestChannel::getWriteMQId() {
    return writeMqId;
}

string MQRequestChannel::getServerOrClient() {
    return (my_side == MQRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
}