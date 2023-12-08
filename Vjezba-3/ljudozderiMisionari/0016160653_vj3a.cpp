#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#ifndef VMISIONARI
#define VMISIONARI 1
#endif

#ifndef VLJUDOZDERI 
#define VLJUDOZDERI 0
#endif

int tipPutnikaUcunu[2] = {0, 0};
int obalaNaKojojJeCun = 1;
int brojPutnikaUCunu = 0;
int brojLjudiNaObalama[2][2] = {{0, 0}, {0, 0}};

sem_t cunSemafor;
sem_t poljeSemafora[2][2];
sem_t kSemafor;
sem_t ukrcatiPutnikeNaCun;
sem_t iskrcatiPutnikeSaCuna;

void *dretvenaFunkcijaLjudozder(void *obala);
void *dretvenaFunkcijaMisionar(void *obala);
void *dretvenaFunkcijaSuma(void *argument);
void *dretvenaFunkcijaCun(void *argument);


void *dretvenaFunkcijaSuma(void *argument){
    pthread_t dretvaID;
    do{
        int pseudoslucajnaObala = rand() % 2;
        pthread_create(&dretvaID, NULL, dretvenaFunkcijaLjudozder, &pseudoslucajnaObala);
        usleep(1000000);
        pseudoslucajnaObala = rand() % 2;
        pthread_create(&dretvaID, NULL, dretvenaFunkcijaLjudozder, &pseudoslucajnaObala);
        pseudoslucajnaObala = rand() % 2;
        pthread_create(&dretvaID, NULL, dretvenaFunkcijaMisionar, &pseudoslucajnaObala);
        usleep(1000000);
    } while (1);

    return NULL;
}

void *dretvenaFunkcijaCun(void *argument){
    obalaNaKojojJeCun = 1;
    do{
        sem_wait(&ukrcatiPutnikeNaCun);
        usleep(1000000);

        printf("Prevezeni: ");

        obalaNaKojojJeCun = 1 - obalaNaKojojJeCun;
        usleep(2000000);
        for (int i = 0; i < brojPutnikaUCunu; i++){
            sem_post(&cunSemafor);
        }
        sem_wait(&iskrcatiPutnikeSaCuna);

        printf("\n");

        for (int i = 0; i < brojLjudiNaObalama[obalaNaKojojJeCun][VMISIONARI]; i++){
            sem_post(&poljeSemafora[obalaNaKojojJeCun][VMISIONARI]);
        }

        for (int i = 0; i < brojLjudiNaObalama[obalaNaKojojJeCun][VLJUDOZDERI]; i++){
            sem_post(&poljeSemafora[obalaNaKojojJeCun][VLJUDOZDERI]);
        }

    } while (1);
}

void *dretvenaFunkcijaLjudozder(void *obala){
    int pseudoslucajnaObala = *((int *)obala);
    sem_wait(&kSemafor);

    brojLjudiNaObalama[pseudoslucajnaObala][VLJUDOZDERI]++;
    bool cunjProvjera = false;

    while (!cunjProvjera){
        if (obalaNaKojojJeCun == pseudoslucajnaObala && brojPutnikaUCunu < 7 && tipPutnikaUcunu[VMISIONARI] > tipPutnikaUcunu[VLJUDOZDERI]){
            cunjProvjera = true;
            brojLjudiNaObalama[pseudoslucajnaObala][VLJUDOZDERI]--;
            brojPutnikaUCunu++;
            tipPutnikaUcunu[VLJUDOZDERI]++;
            if (brojPutnikaUCunu == 3){
                sem_post(&ukrcatiPutnikeNaCun);
            }
            if (brojPutnikaUCunu >= 3 && brojPutnikaUCunu < 7 && brojLjudiNaObalama[pseudoslucajnaObala][VMISIONARI] > 0){
                sem_post(&poljeSemafora[pseudoslucajnaObala][VMISIONARI]);
            }
        }
        else{
            sem_post(&kSemafor);
            sem_wait(&poljeSemafora[pseudoslucajnaObala][VLJUDOZDERI]);
            sem_wait(&kSemafor);
        }
    }

    sem_post(&kSemafor);
    sem_wait(&cunSemafor);
    sem_wait(&kSemafor);

    printf("ljudozder, ");

    brojPutnikaUCunu--;
    tipPutnikaUcunu[VLJUDOZDERI]--;

    if (brojPutnikaUCunu == 0){
        sem_post(&iskrcatiPutnikeSaCuna);
    }

    sem_post(&kSemafor);

    return NULL;
}

void *dretvenaFunkcijaMisionar(void *obala){
    int pseudoslucajnaObala = *((int *)obala);
    sem_wait(&kSemafor);
    brojLjudiNaObalama[pseudoslucajnaObala][VMISIONARI]++;
    bool cunjProvjera = false;

    while (!cunjProvjera){

        if (obalaNaKojojJeCun == pseudoslucajnaObala && brojPutnikaUCunu < 7 && tipPutnikaUcunu[VMISIONARI] >= tipPutnikaUcunu[VLJUDOZDERI]){
            cunjProvjera = true;
            brojLjudiNaObalama[pseudoslucajnaObala][VMISIONARI]--;
            brojPutnikaUCunu++;
            tipPutnikaUcunu[VMISIONARI]++;

            if (brojPutnikaUCunu == 3){
                sem_post(&ukrcatiPutnikeNaCun);
            }

            if (brojPutnikaUCunu >= 3 && brojPutnikaUCunu < 7 && brojLjudiNaObalama[pseudoslucajnaObala][VLJUDOZDERI] > 0){
                sem_post(&poljeSemafora[pseudoslucajnaObala][VLJUDOZDERI]);
            }
        }

        else{
            sem_post(&kSemafor);
            sem_wait(&poljeSemafora[pseudoslucajnaObala][VMISIONARI]);
            sem_wait(&kSemafor);
        }
    }
    sem_post(&kSemafor);
    sem_wait(&cunSemafor);
    sem_wait(&kSemafor);

    printf("misionar, ");

    tipPutnikaUcunu[VMISIONARI]--;
    brojPutnikaUCunu--;

    if (brojPutnikaUCunu == 0){
        sem_post(&iskrcatiPutnikeSaCuna);
    }

    sem_post(&kSemafor);

    return NULL;
}

void inicializacijaSemafora(int ponavljanja){

    for (int i = 0; i < ponavljanja; i++){
        for (int j = 0; j < usleep(1000000); j++){
            sem_init(&poljeSemafora[i][j], 0, 0);
        }
    }

    sem_init(&cunSemafor, 0, 0);
    sem_init(&kSemafor, 0, 1);
    sem_init(&ukrcatiPutnikeNaCun, 0, 0);
    sem_init(&iskrcatiPutnikeSaCuna, 0, 0);
}

void unistavanjeSemafora(int ponavljanja){
    for (int i = 0; i < ponavljanja; i++){
        for (int j = 0; j < ponavljanja; j++){
            sem_destroy(&poljeSemafora[i][j]);
        }
    }

    sem_destroy(&ukrcatiPutnikeNaCun);
    sem_destroy(&kSemafor);
    sem_destroy(&iskrcatiPutnikeSaCuna);
    sem_destroy(&cunSemafor);
}

int main(){

    pthread_t dretvaID;
    int ponavljanja = 2;

    inicializacijaSemafora(ponavljanja);

    pthread_create(&dretvaID, NULL, dretvenaFunkcijaCun, 0);
    dretvenaFunkcijaSuma(NULL);

    unistavanjeSemafora(ponavljanja);
    return 0;
}