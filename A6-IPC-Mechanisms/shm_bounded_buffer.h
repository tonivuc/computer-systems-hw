//
// Created by toni on 11/20/18.
//

#ifndef A6_IPC_MECHANISMS_SHM_BOUNDED_BUFFER_H
#define A6_IPC_MECHANISMS_SHM_BOUNDED_BUFFER_H


class SHMBoundedBuffer {
    int shmid; // shared memory segment ID
    KernelSemaphore full(0);

    KernelSemaphore empty(1); // Buffer size=1 is fine;
};


#endif //A6_IPC_MECHANISMS_SHM_BOUNDED_BUFFER_H
