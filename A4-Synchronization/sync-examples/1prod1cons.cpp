#include <iostream>
#include <stdio.h>
#include <queue>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

int buffer=-1;
pthread_cond_t prod_done, cons_done; 
pthread_mutex_t m;
bool data_taken = true; //mandatory predicate
void* producer (void * args){
	int count = 0;
	while (true){
		pthread_mutex_lock (&m);
		while (!data_taken)	// cond_wait does not work w/o predicate 
			pthread_cond_wait (&cons_done, &m);
		usleep (rand() % 50000);
		buffer = count++;		// put the count in the buffer
		cout << "Producer put >>>>" << buffer << endl;
		data_taken = false;
		pthread_mutex_unlock (&m);
		pthread_cond_signal (&prod_done);
	}
}
void* consumer (void * args){
	while (true){
		pthread_mutex_lock (&m);
		while (data_taken)
			pthread_cond_wait (&prod_done, &m);
		cout << "Consumer Got <<<<<<<<<<" << buffer << endl;
		data_taken = true;
		pthread_mutex_unlock (&m);
		pthread_cond_signal (&cons_done);
	}
}
int main (){
	pthread_t prod, con;
	pthread_mutex_init (&m, 0);
	pthread_cond_init (&prod_done, 0);
	pthread_cond_init (&cons_done, 0);
	
	pthread_create (&prod, 0, producer, 0);
	pthread_create (&con, 0, consumer, 0);
	
	pthread_join (prod, 0);
	pthread_join (con, 0);
}

