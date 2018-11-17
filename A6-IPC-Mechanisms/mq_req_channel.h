//
// Created by toni on 11/16/18.
//

#ifndef A6_IPC_MECHANISMS_MQ_REQ_CHANNEL_H
#define A6_IPC_MECHANISMS_MQ_REQ_CHANNEL_H

#import "requestchannel.h"
#include <sys/ipc.h>
#include <sys/msg.h>


struct my_msgbuf {
    long mtype;
    char mtext[200];
};

class mq_req_channel : public RequestChannel {

    struct my_msgbuf buf;
    //buf.mtype = 1;
    key_t key = ftok ("a.txt", 100); // create a pseudo-random key
    int msqid = msgget(key, 0644 | IPC_CREAT); // create the msg queue
    if (msqid < 0){
        perror ("Message Queue could not be created");
        return 0;
    }
    printf ("Message Queued ID: %ld\n", msqid);
    // int msqid2 = msgget(key, 0644 | IPC_CREAT); // create the msg queue
    // printf ("Message Queued ID: %ld\n", msqid2);

    while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
        int len = strlen(buf.mtext);
        if (msgsnd(msqid, &buf, len+1, 0) == -1)
            perror ("msgsnd");
    }
    //msgctl(msqid, IPC_RMID, NULL);  // delete the msg queue
    return 0;
};


#endif //A6_IPC_MECHANISMS_MQ_REQ_CHANNEL_H
