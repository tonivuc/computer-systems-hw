#include "BoundedBuffer.h"
#include <string>
#include <queue>
using namespace std;

BoundedBuffer::BoundedBuffer(int _cap) {
    pthread_mutex_init (&m, 0);
    pthread_cond_init (&prod_done, 0);
    pthread_cond_init (&cons_done, 0);
    maxSize = _cap; //TODO: Make this const.
}

BoundedBuffer::~BoundedBuffer() {
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&prod_done);
    pthread_cond_destroy(&cons_done);

}

int BoundedBuffer::size() {
    //No point adding mutex here. If I'm using the size, I should be doing it inside the mutex of push or pop to keep it all atomic.
	return q.size();
}

//Producer goes:
//PUSH John
//PUSH Jane
//PUSH Joe (WAIT)
//POP John
//PUSH Joe

//Producer
void BoundedBuffer::push(string str) {
	/*
	Is this function thread-safe??? Does this automatically wait for the pop() to make room 
	when the buffer if full to capacity???
	*/

    pthread_mutex_lock(&m);
    while (size() == maxSize){ //The correct solution for this is of course to check if the wakeup was actually legit before proceding. (Because pthread wait is sometimes woken even if it wasn't signaled to be woken)
        pthread_cond_wait(&cons_done, &m); //Wait until a consumer takes out something to free space
        //DEADLOCK
    }
    // now consume
    q.push (str);
    pthread_mutex_unlock(&m);
    pthread_cond_signal (&prod_done); // send signal to Producer(s) that we have consumed. (V++)

}

//Consumer
string BoundedBuffer::pop() {
	/*
	Is this function thread-safe??? Does this automatically wait for the push() to make data available???
	*/

    pthread_mutex_lock (&m);
    //We want code to stop if there is nothing to pop ye?
    while (size() == 0) {//While empty
        pthread_cond_wait(&prod_done, &m); //Wait for push to signal it has produced something
    }

	string s = q.front();
	q.pop();;
    pthread_mutex_unlock(&m);
    pthread_cond_signal (&cons_done); //There is space now

	return s;
}
