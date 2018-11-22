//
// Created by toni on 11/20/18.
//


#include "kernel_semaphore.h"


KernelSemaphore::KernelSemaphore() {}

KernelSemaphore::KernelSemaphore(short value, key_t key) {

    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    struct sembuf sb = {0,value,0};
    semop(semid, &sb, 1);
}


KernelSemaphore::~KernelSemaphore() {
    semctl(semid, 0, IPC_RMID);
}

void KernelSemaphore::P() {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);

}

void KernelSemaphore::V() {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}

