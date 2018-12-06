//
// Created by toni on 11/16/18.
//

#include <cstring>
#include "network_req_channel.h"
#include <fstream>

NetworkRequestChannel::NetworkRequestChannel(const std::string host_name, char* port, const Side _side) {
    myHostName = host_name;
    my_side = _side;


    if (_side == SERVER_SIDE) {
          // listen on sock_fd, new connection on new_fd
        struct addrinfo hints, *serv; //Serv contains portnumber because of getaddrinfo
        struct sockaddr_storage their_addr; // connector's address information
        socklen_t sin_size;
        char s[INET6_ADDRSTRLEN];
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // use my IP

        if ((rv = getaddrinfo(NULL, port, &hints, &serv)) != 0) { //Needed for the datastructures (port number)
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            EXITONERROR("getaddrinfo");
        }
        if ((mainsockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
            EXITONERROR("server: socket");
        }
        if (bind(mainsockfd, serv->ai_addr, serv->ai_addrlen) == -1) {
            close(mainsockfd);
            EXITONERROR("server: bind");
        }
        freeaddrinfo(serv); // all done with this structure

        if (listen(mainsockfd, 20) == -1) {
            EXITONERROR("listen");
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
        if ((status = getaddrinfo(myHostName.c_str(), port, &hints, &res)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
            exit(-1);
            //return -1;
        }

        // make a socket:
        mainsockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (mainsockfd < 0)
        {
            perror ("Error creating socket\n");
            exit(-1);
        }

        // connect!
        if (connect(mainsockfd, res->ai_addr, res->ai_addrlen)<0)
        {
            perror ("connect error\n");
            exit(-1);
        }
    }
}

int NetworkRequestChannel::acceptWrap() { // struct sockaddr *their_addr, socklen_t *addrlen
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    sin_size = sizeof their_addr;
    int new_fd = accept(mainsockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
        perror("accept");
        //continue;
    }
    printf("server: got connection\n");
    return new_fd;
}

int NetworkRequestChannel::cwrite(string msg, int socketfd) {

    //printf ("Now Attempting to send a message to the server\n", server_name);

    if (send(socketfd, msg.c_str(), strlen(msg.c_str())+1, 0) == -1) { //int sockfd, const void *msg, int len, int flags
        perror("send");
        exit(1);
    }

    return 0;
}

string NetworkRequestChannel::cread(int sockfd) {

    string clientOrServer = (my_side == NetworkRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
    char buf[200];

    if (recv (sockfd, buf, sizeof(buf), 0) == -1) {
        EXITONERROR("recv");
    }

    string str(buf);
    return str;
}



NetworkRequestChannel::~NetworkRequestChannel() {
    string clientOrServer = (my_side == NetworkRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
    //cout << "DELETING A "<< clientOrServer<< " MESSAGE QUEUE with the MQ read: "<<readMqId<< " and write:"<<writeMqId<<endl;
    close(mainsockfd); //Server will close the main socket, and the client will close his socket (in both directions)
}


string NetworkRequestChannel::getServerOrClient() {
    return (my_side == NetworkRequestChannel::SERVER_SIDE) ? "SERVER" : "CLIENT";
}//
// Created by toni on 11/29/18.
//

int NetworkRequestChannel::getMainFD() {
    return  mainsockfd;
}


