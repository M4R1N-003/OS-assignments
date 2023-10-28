#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int Id; /* Identifikacijski broj segmenta */
int *ZajednickaPolja;

struct PoljeTipa1 {
    int polje[30];
};

struct PoljeTipa2 {
    int zbroj;
    int aritmeticka_sredina;
};

void PrviTipProces(int id, struct PoljeTipa1 *polje) {
    for (int i = 0; i < 30; i++) {
        polje->polje[i] = rand() % 100;
    }
}

void DrugiTipProces(int id, struct PoljeTipa1 *polje, struct PoljeTipa2 *rezultati) {
    int zbroj = 0;
    for (int i = 0; i < 30; i++) {
        zbroj += polje->polje[i];
    }
    rezultati->zbroj = zbroj;
    rezultati->aritmeticka_sredina = zbroj / 30;

    printf("Proces %d: Zbroj: %d\n", id, rezultati->zbroj);
    printf("Proces %d: Aritmeticka sredina: %d\n", id, rezultati->aritmeticka_sredina);
}

int main(int argc, char *argv[]) {

    int M = atoi(argv[1]);

    srand(time(0));

    Id = shmget(IPC_PRIVATE, M * (sizeof(struct PoljeTipa1) + sizeof(struct PoljeTipa2)), 0600);

    ZajednickaPolja = (int *)shmat(Id, NULL, 0);

    for (int i = 0; i < M*2; i++) {
        struct PoljeTipa1 *polje1 = (struct PoljeTipa1 *)(ZajednickaPolja + i * (sizeof(struct PoljeTipa1) + sizeof(struct PoljeTipa2)));
        struct PoljeTipa2 *polje2 = (struct PoljeTipa2 *)(ZajednickaPolja + i * (sizeof(struct PoljeTipa1) + sizeof(struct PoljeTipa2) + sizeof(struct PoljeTipa1)));

        PrviTipProces(i, polje1);
        if (fork() == 0) {
            DrugiTipProces(i, polje1, polje2);
            exit(0);
        }
    }

    for (int i = 0; i < M; i++) {
        wait(NULL);
    }

    shmdt(ZajednickaPolja);
    shmctl(Id, IPC_RMID, NULL);

    return 0;
}
