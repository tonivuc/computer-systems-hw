//
// Created by toni on 11/20/18.
//

#include "kernel_semaphore.h"

KernelSemaphore::KernelSemaphore() {}

KernelSemaphore::KernelSemaphore(int value, int seed) {
    key_t key = ftok ("a.txt", seed);
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    struct sembuf sb = {0,value,0};
    semop(semid, &sb, 1);
}


KernelSemaphore::~KernelSemaphore() {

}

void KernelSemaphore::P() {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1)

}

void KernelSemaphore::V() {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);

}

