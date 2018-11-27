//
// Created by toni on 11/20/18.
//

#include <stdlib.h>
#include "kernel_semaphore.h"
#include <iostream>

using namespace std;

KernelSemaphore::KernelSemaphore() {}

KernelSemaphore::KernelSemaphore(short value, key_t key) {


        semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666); //IPC_EXCL
        if (semid < 0) {
            perror("semget");
            do {
                semid = semget(key, 1, IPC_CREAT | 0666); //IPC_EXCL
                //cout << "Error in kernelsemaphore! semid: "<<semid<<endl;
                //exit(0);
            } while (semid < 0);
        }


    struct sembuf sb = {0,value,0}; //VALUE HAS TO BE POSITIVE. Only run semop when value is positive.
    if (value > 0) semop(semid, &sb, 1);
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

int KernelSemaphore::getSemId() {
    return semid;
}

