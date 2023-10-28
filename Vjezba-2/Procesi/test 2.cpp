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

void generiranje(int indeks, int* polje) {
    srand(time(0)+indeks);
    cout << "Generirani brojevi polja " << indeks << " su: "; 
    for (int i = 0; i < N; i++) {
        polje[i] = rand()%100;
        cout << polje[i] << " ";
    }
    cout << endl;
}
int brojac=0;
void racunanje(int indeks, int* polje) {
    int sum = 0;

    for (int i = 0; i < N; i++) {
        sum = sum + polje[i];
    }

    double average = (double)sum / N;

    cout << "Polje" << indeks << " zbroj = " << sum << endl;
    cout << "Polje" << indeks << " aritm_sredina = " << average << endl;
}

key_t kljuc;

struct dvodimenzionalnoPolje {
    int (*polje)[N];
} *dijeljenoPolje = new dvodimenzionalnoPolje;

int main(int argc, char* argv[]) {
    int M = atoi(argv[1]);

    kljuc = shmget(IPC_PRIVATE, M * N * sizeof(int), 0600);

    dijeljenoPolje->polje = (int(*)[N])shmat(kljuc, NULL, 0);

    for (int i = 1; i <= 2 * M; i++) {
        if (i % 2 == 1) {
            if (fork() == 0) {
                generiranje(i, dijeljenoPolje->polje[i - 1]);
                exit(0);
            }
        } else {
            if (fork() == 0) {
                racunanje(i, dijeljenoPolje->polje[i - M - 1]);
                exit(0);
            }
        }
    }

    for (int i = 0; i < 2 * M; i++) {
        wait(NULL);
    }

    return 0;
}