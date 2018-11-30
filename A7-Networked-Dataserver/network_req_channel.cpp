//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "network_req_channel.h"
#include <fstream>

NetworkRequestChannel::NetworkRequestChannel(const std::string host_name, int port, const Side _side) {
    my_name = _name;
    my_side = _side;


    if (_side == SERVER_SIDE) {
          // listen on sock_fd, new connection on new_fd
        struct addrinfo hints, *serv;
        struct sockaddr_storage their_addr; // connector's address information
        socklen_t sin_size;
        char s[INET6_ADDRSTRLEN];
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // use my IP

        if ((rv = getaddrinfo(NULL, port, &hints, &serv)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return -1;
        }
        if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
            perror("server: socket");
            return -1;
        }
        if (bind(sockfd, serv->ai_addr, serv->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            return -1;
        }
        freeaddrinfo(serv); // all done with this structure

        if (listen(sockfd, 20) == -1) {
            perror("listen");
            exit(1);
        }

        while(1) {  // main accept() loop
            sin_size = sizeof their_addr;
            new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
            if (new_fd == -1) {
                perror("accept");
                continue;
            }
            //inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
            printf("server: got connection\n");
        }

    }
    else { //Client-side
        struct addrinfo hints, *res;

        // first, load up address structs with getaddrinfo():

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        int status;
        //getaddrinfo("www.example.com", "3490", &hints, &res);
        if ((status = getaddrinfo(server_name, port, &hints, &res)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
            exit(-1);
            //return -1;
        }

        // make a socket:
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
        {
            perror ("Error creating socket\n");
            exit(-1);
            //return -1;
        }

        // connect!
        if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0)
        {
            perror ("connect error\n");
            exit(-1);
            //return -1;
        }
        printf ("Successfully connected to the server %s\n", server_name);
    }
}

int NetworkRequestChannel::cwrite(string msg) {


    if (send(sockfd, msg, strlen(msg.c_str())+1, 0) { //int sockfd, const void *msg, int len, int flags
        perror("send");
    }

    //OLD stuff:
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

string NetworkRequestChannel::cread() {

    string clientOrServer = (my_side == NetworkRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";

    recv (sockfd, buf, 1024, 0);
    printf ("Received %s from the server\n", buf);

    //Old stuff:
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



NetworkRequestChannel::~NetworkRequestChannel() {
    string clientOrServer = (my_side == NetworkRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
    //cout << "DELETING A "<< clientOrServer<< " MESSAGE QUEUE with the MQ read: "<<readMqId<< " and write:"<<writeMqId<<endl;
    close(new_fd);
}


string NetworkRequestChannel::getServerOrClient() {
    return (my_side == NetworkRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
}//
// Created by toni on 11/29/18.
//


