//
// Created by toni on 11/20/18.
//

#ifndef A6_IPC_MECHANISMS_SHM_BOUNDED_BUFFER_H
#define A6_IPC_MECHANISMS_SHM_BOUNDED_BUFFER_H

#include "kernel_semaphore.h"
#include <string>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

class SHMBoundedBuffer {
    int shmid; // shared memory segment ID
    char* buffer;
    string filename;
    KernelSemaphore* f;
    KernelSemaphore* e; // Buffer size=1 is fine;

public:
    SHMBoundedBuffer(string name);
    void push(string s);
    string pop();
    ~SHMBoundedBuffer();
};


#endif //A6_IPC_MECHANISMS_SHM_BOUNDED_BUFFER_H
