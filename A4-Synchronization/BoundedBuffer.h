#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

using namespace std;

class BoundedBuffer {
private:
    pthread_mutex_t m;
    pthread_cond_t prod_done, cons_done;
	queue<string> q;
	int maxSize;
public:
    BoundedBuffer(int);
	~BoundedBuffer();
	int size();
    void push (string);
    string pop();
};

#endif /* BoundedBuffer_ */
