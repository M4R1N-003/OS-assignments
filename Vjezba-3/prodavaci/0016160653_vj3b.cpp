#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>
#include <signal.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

using namespace std;

#ifndef N
#define N 5
#endif

int semaforID;
int idShare;
key_t kljucSheraneMemorije;

struct SharedMemory {
    int M[N];
    int ulazakSem;
    int izlazSem;
    int ukupno;
};

SharedMemory *shareVar;

void brisanjeMemorije(int num) {
    shmdt(shareVar);
    shmctl(idShare, IPC_RMID, NULL);
    semctl(semaforID, 0, IPC_RMID);
}

void semCekanje(int semaforID, int vrijednostSemafora) {
    sembuf semBuferVar;
    semBuferVar.sem_num = vrijednostSemafora;
    semBuferVar.sem_op = -1;
    semBuferVar.sem_flg = 0;
    semop(semaforID, &semBuferVar, 1);
}

void semSignalizacija(int semaforID, int vrijednostSemafora) {
    sembuf semBuferVar;
    semBuferVar.sem_num = vrijednostSemafora;
    semBuferVar.sem_op = 1;
    semBuferVar.sem_flg = 0;
    semop(semaforID, &semBuferVar, 1);
}

int potrosacFun(int zbrojBrojeva){
    while (shareVar->ukupno > 0) {
        semCekanje(semaforID, 2);
        semCekanje(semaforID, 0);

        shareVar->izlazSem = (shareVar->izlazSem) % N;
        int zaprimljeniBroj = shareVar->M[shareVar->izlazSem];
        printf("Potrošač prima: %d \n", zaprimljeniBroj);
        shareVar->izlazSem++;
        usleep(250000);
        (shareVar->ukupno)--;
        zbrojBrojeva += zaprimljeniBroj;

        semSignalizacija(semaforID, 0);
        semSignalizacija(semaforID, 1);
    }
    return zbrojBrojeva;
}

void alociranjeMemorije(){
    idShare = shmget(kljucSheraneMemorije, sizeof(SharedMemory), 0600);
    shareVar = (SharedMemory *)shmat(idShare, 0, 0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Netočan broj argumenata naredbenog retka! 2 argumenta potrebna: broj broj proizvođača i broj vrijednosti koje će proslijediti\n";
        return 1;
    }

    sigset(SIGINT, brisanjeMemorije);

    alociranjeMemorije();

    int brojProizvodjac = atoi(argv[1]);
    int brojVrijednosti = atoi(argv[2]);
    int i=0;
    while(i<N){
        shareVar->M[i] = 0;
        i++;
    }

    shareVar->ulazakSem = 0;
    shareVar->izlazSem = 0;
    shareVar->ukupno = brojProizvodjac * brojVrijednosti;

    semaforID = semget(kljucSheraneMemorije, 3, 0600);
    semctl(semaforID, 0, SETVAL, 1);
    semctl(semaforID, 1, SETVAL, N);
    semctl(semaforID, 2, SETVAL, 1); 

    for (int i = 0; i < brojProizvodjac; ++i) {
        srand(time(NULL) + i);

        if (fork() == 0) {
            int poslani_brojevi = 0;
            while (poslani_brojevi < brojVrijednosti) {
                semCekanje(semaforID, 1);
                semCekanje(semaforID, 0);

                int pseudoBroj = rand() % 1000 + 1;
                shareVar->M[shareVar->ulazakSem] = pseudoBroj;
                shareVar->ulazakSem = (shareVar->ulazakSem + 1) % N;

                printf("Proizvođač broj: %d šalje: %d\n", i + 1, pseudoBroj);

                semSignalizacija(semaforID, 0);
                semSignalizacija(semaforID, 2);

                poslani_brojevi++;
            }

            printf("Proizvođač broj: %d je završio sa slanjem!\n", i + 1);
            _exit(0);
        }
    }

    if (fork() == 0) {
        int zbrojBrojeva = 0;
        zbrojBrojeva = potrosacFun(zbrojBrojeva);

        printf("Zbroj brojeva koje je primio potrošač je: %d \n", zbrojBrojeva);
        _exit(0);
    }

    for (int i = 0; i <= brojProizvodjac; i++) {
        wait(NULL);
    }

    brisanjeMemorije(0);

    return 0;
}
