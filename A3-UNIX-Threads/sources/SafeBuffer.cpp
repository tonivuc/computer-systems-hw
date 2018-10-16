#include "SafeBuffer.h"
#include <string>
#include <queue>
#include <iostream>

using namespace std;

SafeBuffer::SafeBuffer() {
	pthread_mutex_init (&m, NULL); 
}

SafeBuffer::~SafeBuffer() {
	pthread_mutex_destroy (&m);
}

int SafeBuffer::size() {
	/*
	Is this function thread-safe???
	Make necessary modifications to make it thread-safe
	*/
	pthread_mutex_lock (&m);
	int size = q.size();
	pthread_mutex_unlock (&m);
    return size;
}

void SafeBuffer::push(string str) {
	/*
	Is this function thread-safe???
	Make necessary modifications to make it thread-safe
	*/
	pthread_mutex_lock (&m);
	std::cout << "pushing "<<str<<" to SafeBuffer\n";
	q.push (str);
	std::cout << "size: "<<q.size()<<"\n";
	pthread_mutex_unlock (&m);
}

string SafeBuffer::pop() {
	/*
	Is this function thread-safe???
	Make necessary modifications to make it thread-safe
	*/
	pthread_mutex_lock (&m);
	string s = q.front();
	q.pop();
	pthread_mutex_unlock (&m);
	return s;
}
