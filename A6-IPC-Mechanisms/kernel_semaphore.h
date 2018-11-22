//
// Created by toni on 11/20/18.
//

#ifndef A6_IPC_MECHANISMS_KERNEL_SEMAPHORE_H
#define A6_IPC_MECHANISMS_KERNEL_SEMAPHORE_H
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>


class KernelSemaphore {
private :
/* INTERNAL DATA STRUCTURES */
    int semid;
public :
/* CONSTRUCTOR/DESTRUCTOR */
    KernelSemaphore(short value, int seed);

    KernelSemaphore();

    ~KernelSemaphore(); // make sure to remove all allocated resources
    void P() ; /* Acquire Lock*/
    void V() ; /* Release Lock */
} ;


#endif //A6_IPC_MECHANISMS_KERNEL_SEMAPHORE_H
