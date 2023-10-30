#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <ctime>
#include <wait.h>
#include <signal.h>

#ifndef N
#define N 30
#endif

using namespace std;

key_t kljucic;
int (*polje)[N];

void generiranje(int indeks) {
    srand(time(0)+indeks);
    for (int i = 0; i < N; i++) {
        polje[indeks][i] = rand()%5000;
    }
}

void racunanje(int indeks) {
    int sum = 0;

    for (int i = 0; i < N; i++) {
        sum = sum + polje[indeks][i];
    }

    double average = (double)sum / N;
    cout << "Polje " << indeks+1 << " zbroj = " << sum << endl;
    cout << "Polje " << indeks+1 << " aritm_sredina = " << average << endl;
}

void prekidna_rutina(int sig){
    shmdt(polje);
    shmctl(kljucic, IPC_RMID, NULL);
}


int main(int argc, char* argv[]) {
    int M = atoi(argv[1]);

    kljucic = shmget(IPC_PRIVATE, sizeof(polje)*M*N, 0600);
    polje = (int(*)[N])shmat(kljucic, NULL, 0);

    sigset(SIGINT, prekidna_rutina);

    for (int i = 0; i < M; i++) {
        if (fork() == 0) {
            generiranje(i);
            exit(0);
        }
        usleep(100);
        if (fork() == 0) {
            racunanje(i);
            exit(0);
        }
    }

    for (int i = 0; i < 2 * M; i++) {
        wait(NULL);
    }

    shmdt(polje);
    shmctl(kljucic, IPC_RMID, NULL);

    return 0;
}