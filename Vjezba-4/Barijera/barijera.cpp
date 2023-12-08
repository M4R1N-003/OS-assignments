#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

pthread_mutex_t monitor;
pthread_cond_t array;
int counter, thread_number;

bool Check(int argc) {
	if (argc < 2 || argc > 2) {
		cout << "Pogresan unos! Potreban je 1 argument!" << endl; 
		return false;
    }
    return true;
}

void *Barrier(void* arg) {
		int *ID_number = (int*) arg;
		int number = 0;
		pthread_mutex_lock (&monitor);
		counter++;
		
		if (counter < thread_number) {
			cout << "Dretva " << *ID_number << ". unesite broj: ";
			cin >> number;
			pthread_cond_wait (&array, &monitor);
		}
		
		else {
			cout << "Dretva " << *ID_number << ". unesite broj: ";
			cin >> number;
			cout << endl;
			counter = 0;
			pthread_cond_broadcast (&array);
		}
		
		pthread_mutex_unlock (&monitor);
		pthread_mutex_lock (&monitor);
		counter++;
		
		if (counter < thread_number) {
			cout << "Dretva " << *ID_number << ". uneseni broj je: " << number << endl;
			pthread_cond_wait (&array, &monitor);
		}
		
		else {
			cout << "Dretva " << *ID_number << ". uneseni broj je: " << number << endl;
			counter = 0;
			pthread_cond_broadcast(&array);
		}
		
		pthread_mutex_unlock(&monitor);
}

void Remove_Monitors (int sig) {
	pthread_mutex_destroy (&monitor);
	pthread_cond_destroy (&array);
	exit(0);
}

int main (int argc, char** argv) {
	int prvi_argument;
	
	if (Check(argc)) {
    	prvi_argument = atoi (argv[1]);
	}
	else {
		return 0;
	}
	
	cout << endl;
	sigset (SIGINT, Remove_Monitors);
		
	counter = 0;
	thread_number = prvi_argument;
	pthread_mutex_init (&monitor, NULL);
	pthread_cond_init (&array, NULL);
	pthread_t tid [prvi_argument];
	
	int variable_1 [prvi_argument];
	
	for (int variable_2 = 0; variable_2 < thread_number; variable_2++) {
		variable_1 [variable_2] = variable_2;
		if (pthread_create (&tid[variable_2], NULL, Barrier, &variable_1 [variable_2]) != 0) {
			cout << "Pogreska!" << endl;
			exit(1);
		}
	}
	
	for(int variable_3 = 0; variable_3 < thread_number; variable_3++) {
		pthread_join(tid[variable_3], NULL);
	}
	
	cout << endl;
	cout << "Program je zavrsio!" << endl;
	cout << endl;
	
	Remove_Monitors (0);
}
