#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAKSIMALNI_KLIJENTI 20
#define BROJ_MJESTA 5
#define RADNO_VRIJEME 10

pthread_mutex_t monitorZaBrijaca = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t uvjetBrijaca = PTHREAD_COND_INITIALIZER;

int otvorenaTrgovina = 1;
int dostupnaSjedala = 0;
int zadovoljniKlijenti = 0;
int programZavrsen = 0;

void *dretvaBrijac(void *arg);
void *dretvaKlijent(void *arg);
void *dretvaGeneratoraKlijenata(void *arg);

void KreiranjeZavrsavanjeDretvi() {
    pthread_t brijacId;
    pthread_create(&brijacId, NULL, dretvaBrijac, NULL);

    pthread_t generatorKlijenataId;
    pthread_create(&generatorKlijenataId, NULL, dretvaGeneratoraKlijenata, NULL);

    sleep(RADNO_VRIJEME);

    pthread_mutex_lock(&monitorZaBrijaca);
    otvorenaTrgovina = 0;
    programZavrsen = 1;
    pthread_mutex_unlock(&monitorZaBrijaca);

    pthread_join(brijacId, NULL);
    pthread_join(generatorKlijenataId, NULL);
}

void generiranjeKlijenata(int idKlijenta) {
    while (idKlijenta < MAKSIMALNI_KLIJENTI) {
        pthread_t dretvaKlijentaId;
        pthread_create(&dretvaKlijentaId, NULL, dretvaKlijent, (void *)&idKlijenta);
        idKlijenta++;

        usleep(rand() % 1000000);
    }
}

void *dretvaGeneratoraKlijenata(void *arg) {
    int idKlijenta = 0;
    generiranjeKlijenata(idKlijenta);
    pthread_exit(NULL);
}

void *dretvaKlijent(void *arg) {
    int idKlijenta = *((int *)arg);

    pthread_mutex_lock(&monitorZaBrijaca);
    printf("        Klijent(%d): Želim brijanje\n", idKlijenta);

    if (otvorenaTrgovina && dostupnaSjedala < BROJ_MJESTA) {
        dostupnaSjedala++;
        printf("        Klijent(%d): Ulazim u čekaonicu. (%d)\n", idKlijenta, dostupnaSjedala);
        pthread_cond_signal(&uvjetBrijaca);
        pthread_mutex_unlock(&monitorZaBrijaca);

        usleep(rand() % 1000000);

        pthread_mutex_lock(&monitorZaBrijaca);
        pthread_mutex_unlock(&monitorZaBrijaca);

        usleep(rand() % 1000000);
    } else {
        printf("        Klijent(%d): Salon je zatvoren ili nema dostupnih sjedala. Odlazim.\n", idKlijenta);
        pthread_mutex_unlock(&monitorZaBrijaca);
    }

    pthread_exit(NULL);
}

void *dretvaBrijac(void *arg) {
    while (1) {
        pthread_mutex_lock(&monitorZaBrijaca);

        while (!otvorenaTrgovina && dostupnaSjedala == 0) {
            if (programZavrsen) {
                printf("Brijac: Zatvaram trgovinu.\n");
                pthread_mutex_unlock(&monitorZaBrijaca);
                return NULL;
            }
            printf("Brijac: Spavam dok klijenti ne dođu.\n");
            pthread_cond_wait(&uvjetBrijaca, &monitorZaBrijaca);
        }

        if (dostupnaSjedala > 0) {
            printf("Brijac: Počinjem raditi\n");
            dostupnaSjedala--;
            zadovoljniKlijenti++;
            printf("        Klijent(%d): Brijac me brije\n", zadovoljniKlijenti);
            pthread_mutex_unlock(&monitorZaBrijaca);

            usleep(rand() % 1000000);

            printf("Brijac: Klijent gotov\n");
        } else {
            pthread_mutex_unlock(&monitorZaBrijaca);
            usleep(1000000);
        }
    }
    return NULL;
}

void sigHandle(int signum) {
    printf("\nPrimio SIGINT. Zatvaram salon.\n");
    pthread_mutex_lock(&monitorZaBrijaca);
    otvorenaTrgovina = 0;
    programZavrsen = 1;
    pthread_mutex_unlock(&monitorZaBrijaca);

    pthread_mutex_destroy(&monitorZaBrijaca);
    pthread_cond_destroy(&uvjetBrijaca);

    exit(0);
}

int main() {
    srand(time(NULL));
    printf("Brijac: Otvaram brijačnicu\n");

    signal(SIGINT, sigHandle);

    KreiranjeZavrsavanjeDretvi();

    printf("\n------------------------------------------------\n");
    printf("Broj ljudi koji su obrijani: %d\n", zadovoljniKlijenti);
    printf("\n------------------------------------------------\n");

    return 0;
}