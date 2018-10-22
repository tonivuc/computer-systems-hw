#include "BoundedBuffer.h"
#include <string>
#include <queue>
using namespace std;

BoundedBuffer::BoundedBuffer(int _cap) {

}

BoundedBuffer::~BoundedBuffer() {
	
}

int BoundedBuffer::size() {
	return q.size();
}

void BoundedBuffer::push(string str) {
	/*
	Is this function thread-safe??? Does this automatically wait for the pop() to make room 
	when the buffer if full to capacity???
	*/


	//This one mutex is for the entire boundedbuffer? To avoid race conditions?
    pthread_mutex_lock(&mtx);
    while (buffer.size () == 0){ //The correct solution for this is of course to check if the wakeup was actually legit before proceding. (Because pthread wait is sometimes woken even if it wasn't signaled to be woken)
        pthread_cond_wait(&cond, &mtx);
        //V--?
    }
    // now consume
    q.push (str);
    pthread_cond_signal (&cond2); // send signal to Producer(s) that we have consumed. (V++)
    pthread_mutex_unlock(&mtx);
}

string BoundedBuffer::pop() {
	/*
	Is this function thread-safe??? Does this automatically wait for the push() to make data available???
	*/
	string s = q.front();
	q.pop();
	return s;
}
