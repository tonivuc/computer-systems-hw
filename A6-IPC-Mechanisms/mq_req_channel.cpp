//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "mq_req_channel.h"

MQRequestChannel::MQRequestChannel(const std::string _name, const Side _side) {
    my_name = _name;
    my_side = _side;
    string mqFileName = mq_name(WRITE_MODE);

    if (_side == SERVER_SIDE) {
        writeMqId = createQueue(mqFileName.c_str(), WRITE_MODE); //Open mq to write to
        readMqId = createQueue(mqFileName.c_str(), READ_MODE); //Open mq to write to
    }
    else {
        writeMqId = createQueue(mqFileName.c_str(), READ_MODE); //Open mq to write to
        readMqId = createQueue(mqFileName.c_str(), WRITE_MODE); //Open mq to write to
    }
}

int MQRequestChannel::createQueue(string mqFileName, Mode mode) {

    //Programs that want to use the same queue must generate the same key, so they must pass the
    //same parameters to ftok().
    key_t key;
    if (my_side == CLIENT_SIDE) {
        if (mode == READ_MODE)
            key = ftok (mqFileName.c_str(), 'A'); // create a pseudo-random key. 2nd argument is usually some set number
        else
            key = ftok (mqFileName.c_str(), 'B'); // create a pseudo-random key. 2nd argument is usually some set number
    }
    else {
        // SERVER_SIDE
        if (mode == READ_MODE)
            key = ftok (mqFileName.c_str(), 'B'); // create a pseudo-random key. 2nd argument is usually some set number
        else
            key = ftok (mqFileName.c_str(), 'A'); // create a pseudo-random key. 2nd argument is usually some set number
    }

    if (key == -1) {
        perror("Problem getting key");
    }
    int msqid = msgget(key, 0644 | IPC_CREAT); // you want to connect to a queue, or create it if it doesn't exist
    if (msqid < 0){
        perror ("Message Queue could not be created");
        return 0;
    }
    printf ("Message Queued ID: %ld\n", msqid);
    return msqid;

}

std::string MQRequestChannel::mq_name(Mode _mode) {
    std::string qname = "mq_" + my_name;

    /*
    if (my_side == CLIENT_SIDE) {
        if (_mode == READ_MODE)
            qname += "1";
        else
            qname += "2";
    }
    else {
        // SERVER_SIDE
        if (_mode == READ_MODE)
            qname += "2";
        else
            qname += "1";
    }
    */
    return qname;
}

int MQRequestChannel::cwrite(string msg) {

    int msgqID;
    if (my_side==SERVER_SIDE) msgqID = serverToClientMqId;
    else msgqID = clientToServerMqId;

    struct my_msgbuf msgStruct;
    msgStruct.mtype = 1;

    strncpy(msgStruct.mtext, msg.c_str(), MSGMAX);

    int len = strlen(msgStruct.mtext);;
    if (msg.size() > MSGMAX) {
        EXITONERROR ("cwrite message length exceeded");
        return -1;
    }
    if (msgsnd(msgqID, &msg, len+1, 0) == -1) {
        EXITONERROR ("cwrite");
        return -1;
    }
    return 0;
}

string MQRequestChannel::cread() {

    struct my_msgbuf msgStruct;
    msgStruct.mtype = 1;
    int msgqID;
    if (my_side==SERVER_SIDE) msgqID = serverToClientMqId;
    else msgqID = clientToServerMqId;


    if (msgrcv(msgqID, &msgStruct.mtext, sizeof(msgStruct.mtext), 0, 0)<= 0) {
        EXITONERROR ("cread error");
    }

    string s = msgStruct.mtext;
    return s;
}



MQRequestChannel::~MQRequestChannel() {
    msgctl(writeMqId, IPC_RMID, NULL);
    msgctl(readMqId, IPC_RMID, NULL);
}
