#include <iostream>
#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "Semaphore.h"
using namespace std;

int nconsumers = 5;
int buffer=-1;
Semaphore empty (1);
Semaphore full (0);
Semaphore mtx (1); // will use as mutex
int readcount = 0;


void* producer (void * args){
	int count = 0;
	while (true){
		empty.P();		// wait for the buffer to be empty
		readcount = 0;
		buffer = count++;		// put the count in the buffer
		cout << "Producer put >>>>" << buffer << endl;
		for (int i=0; i<nconsumers; i++)
			full.V();
	}
}
void* consumer (void * args){
	while (true){
		full.P();
		mtx.P();
		cout << "Consumer Got <<<<<<<<<<" << buffer << endl;
		usleep (rand() % 50000);
		readcount ++;
		if (readcount == nconsumers)
			empty.V();
		mtx.V();
	}
}
int main (){
	pthread_t prod, con;
	pthread_create (&prod, 0, producer, 0);
	//for (int i=0; i<nconsumers; i++)
	pthread_create (&con, 0, consumer, 0);
	
	pthread_join (prod, 0);
	pthread_join (con, 0);
}

