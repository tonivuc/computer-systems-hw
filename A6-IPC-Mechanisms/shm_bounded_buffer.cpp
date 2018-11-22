//
// Created by toni on 11/20/18.
//



#include "shm_bounded_buffer.h"
#include "shm_req_channel.h"

SHMBoundedBuffer::SHMBoundedBuffer(string name) {

    filename = name;

    std::ofstream file {filename.c_str()}; //Use a vector to delete these later?

    key_t kshm = ftok(name.c_str(), 0);
    key_t kFull = ftok(name.c_str(), 1);
    key_t kEmpty = ftok(name.c_str(), 2);

    if (((kshm == -1) || (kFull==-1) || (kEmpty == -1))) {
        perror("ftok");
        exit(1);
    }

    /* Create the segment: */
    if ((shmid = shmget(kshm, SHM_SIZE, 0666 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    /* attach to the segment to get a pointer to it: */
    buffer = (char*)shmat(shmid, 0, 0);
    if (*buffer == -1) {
        perror("shmat");
        exit(1);
    }

}

SHMBoundedBuffer::~SHMBoundedBuffer() {
    remove(filename.c_str());
}
