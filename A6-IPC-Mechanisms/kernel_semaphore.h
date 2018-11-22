//
// Created by toni on 11/20/18.
//

#ifndef A6_IPC_MECHANISMS_KERNEL_SEMAPHORE_H
#define A6_IPC_MECHANISMS_KERNEL_SEMAPHORE_H


class KernelSemaphore {
private :
/* INTERNAL DATA STRUCTURES */
    int semid;
public :
/* CONSTRUCTOR/DESTRUCTOR */
    KernelSemaphore(int value, int seed);

    KernelSemaphore();

    ~KernelSemaphore(); // make sure to remove all allocated resources
    void P() ; /* Acquire Lock*/
    void V() ; /* Release Lock */
} ;


#endif //A6_IPC_MECHANISMS_KERNEL_SEMAPHORE_H
