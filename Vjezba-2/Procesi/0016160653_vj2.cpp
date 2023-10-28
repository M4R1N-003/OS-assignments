#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <ctime>
#include <wait.h>

#ifndef N
#define N 30
#endif

using namespace std;

key_t kljuc;
int (*polje)[N];

void generiranje(int indeks) {
    srand(time(0)+indeks);
    for (int i = 0; i < N; i++) {
        polje[indeks][i] = rand()%100;
    }
}

void racunanje(int indeks) {
    int sum = 0;

    for (int i = 0; i < N; i++) {
        sum = sum + polje[indeks][i];
    }

    double average = (double)sum / N;
    usleep(1000000);
    cout << "Polje " << indeks+1 << " zbroj = " << sum << endl;
    cout << "Polje " << indeks+1<< " aritm_sredina = " << average << endl;
}


int main(int argc, char* argv[]) {
    int M = atoi(argv[1]);

    kljuc = shmget(IPC_PRIVATE, sizeof(polje)*M*N, 0600);
    polje = (int(*)[N])shmat(kljuc, NULL, 0);

    for (int i = 0; i < M; i++) {
        if (fork() == 0) {
            //cout << "Kreiran proces [" << i << "] za generianje.\n"; 
            generiranje(i);
            exit(0);
        }
        if (fork() == 0) {
            //cout << "Kreiran proces [" << i << "] za računanje.\n";
            racunanje(i);
            exit(0);
        }
    }

    for (int i = 0; i < 2 * M; i++) {
        wait(NULL);
    }

    shmdt(polje);
    shmctl(kljuc, IPC_RMID, NULL);

    return 0;
}