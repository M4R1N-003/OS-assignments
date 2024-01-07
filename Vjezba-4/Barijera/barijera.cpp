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

pthread_mutex_t mutexMonitorMG;
pthread_cond_t poljeUvjeta;
int brojacMon, brojDretveMG;

bool provjeraArgumenata(int brojArgumenataNR) {
	if (brojArgumenataNR < 2 || brojArgumenataNR > 2) {
		cout << "Pogresan unos! Potreban je 1 argument!" << endl; 
		return false;
    }
    return true;
}

void *dretvaBarijera(void* arg) {
		int trenutnaDretva = *((int*) arg);
		int vrijednostDretve = 0;
		pthread_mutex_lock (&mutexMonitorMG);
		brojacMon++;
		
		if (brojacMon < brojDretveMG) {
			cout << "Dretva " << trenutnaDretva << ". unesite broj: ";
			cin >> vrijednostDretve;
			pthread_cond_wait (&poljeUvjeta, &mutexMonitorMG);
		}
		
		else {
			cout << "Dretva " << trenutnaDretva << ". unesite broj: ";
			cin >> vrijednostDretve;
			cout << endl;
			brojacMon = 0;
			pthread_cond_broadcast (&poljeUvjeta);
		}
		
		pthread_mutex_unlock (&mutexMonitorMG);
		pthread_mutex_lock (&mutexMonitorMG);
		brojacMon++;
		
		if (brojacMon < brojDretveMG) {
			cout << "Dretva " << trenutnaDretva << ". uneseni broj je: " << vrijednostDretve << endl;
			pthread_cond_wait (&poljeUvjeta, &mutexMonitorMG);
		}
		
		else {
			cout << "Dretva " << trenutnaDretva << ". uneseni broj je: " << vrijednostDretve << endl;
			brojacMon = 0;
			pthread_cond_broadcast(&poljeUvjeta);
		}
		
		pthread_mutex_unlock(&mutexMonitorMG);

	return NULL;
}

void Remove_Monitors (int sig) {
	pthread_mutex_destroy (&mutexMonitorMG);
	pthread_cond_destroy (&poljeUvjeta);
	exit(0);
}

int main (int brojArgumenataNR, char *poljeArgumenataNR[]) {
	int brojDretvi;
	
	if (provjeraArgumenata(brojArgumenataNR)) {
    	brojDretvi = atoi (poljeArgumenataNR[1]);
	}
	else {
		return 0;
	}
	
	cout << endl;
	sigset (SIGINT, Remove_Monitors);
		
	brojacMon = 0;
	brojDretveMG = brojDretvi;
	pthread_mutex_init (&mutexMonitorMG, NULL);
	pthread_cond_init (&poljeUvjeta, NULL);
	pthread_t idDretva [brojDretvi];
	
	int varijabla1 [brojDretvi];
	
	for (int varijabla2 = 0; varijabla2 < brojDretveMG; varijabla2++) {
		varijabla1 [varijabla2] = varijabla2;
		if (pthread_create (&idDretva[varijabla2], NULL, dretvaBarijera, &varijabla1 [varijabla2]) != 0) {
			cout << "Pogreska!" << endl;
			exit(1);
		}
	}
	
	for(int varijabla3 = 0; varijabla3 < brojDretveMG; varijabla3++) {
		pthread_join(idDretva[varijabla3], NULL);
	}
	
	cout << endl;
	cout << "Program je zavrsio!" << endl;
	cout << endl;
	
	Remove_Monitors (0);
}
