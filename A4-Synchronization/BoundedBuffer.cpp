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
	
}

int BoundedBuffer::size() {
    //No point adding mutex here. If I'm using the size, I should be doing it inside the mutex of push or pop to keep it all atomic.
	return q.size();
}

//Producer
void BoundedBuffer::push(string str) {
	/*
	Is this function thread-safe??? Does this automatically wait for the pop() to make room 
	when the buffer if full to capacity???
	*/

    pthread_mutex_lock(&m);
    while (size() == 0){ //The correct solution for this is of course to check if the wakeup was actually legit before proceding. (Because pthread wait is sometimes woken even if it wasn't signaled to be woken)
        pthread_cond_wait(&cons_done, &m);
        //V--?
    }
    // now consume
    q.push (str);
    pthread_cond_signal (&cond2); // send signal to Producer(s) that we have consumed. (V++)
    pthread_mutex_unlock(&m);
}

//Consumer
string BoundedBuffer::pop() {
	/*
	Is this function thread-safe??? Does this automatically wait for the push() to make data available???
	*/

    pthread_mutex_lock (&m);
    //We want code to stop if there is nothing to pop ye?
    while (size() == 0) //While empty
        pthread_cond_wait (&prod_done, &m); //Wait for push to signal it has produced something

	string s = q.front();
	q.pop();
    pthread_mutex_unlock(&m);
	return s;
}
