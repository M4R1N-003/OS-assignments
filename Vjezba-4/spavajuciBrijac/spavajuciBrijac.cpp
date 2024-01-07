#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define maksimalanBrojKlijenata 20

pthread_mutex_t monitorZaBrijaca;

int otvorenSalon = 1;          
int slobodnihMjesta = 0;    
int zadovoljniKlijenti = 0;   

void *dretvaBrijac(void *arg);
void *dretvaKlijent(void *arg);
void *dretvaKreiranjaKlijenata(void *arg);

void KreiranjeZavrsavanjeDretvi(){
    pthread_t brijacId;
    pthread_create(&brijacId, NULL, dretvaBrijac, NULL);

    pthread_t klijentGeneratorId;
    pthread_create(&klijentGeneratorId, NULL, dretvaKreiranjaKlijenata, NULL);

    pthread_join(brijacId, NULL);
    pthread_join(klijentGeneratorId, NULL);
}

void generiranjeKlijenata(int idKlijenta){
    while (idKlijenta < maksimalanBrojKlijenata) {
        pthread_t dretvaKlijentaId;
        pthread_create(&dretvaKlijentaId, NULL, dretvaKlijent, (void *)&idKlijenta);
        idKlijenta++;

        usleep(rand() % 1000000);
    }
}

void *dretvaKreiranjaKlijenata(void *arg) {
    int idKlijenta = 0;
    generiranjeKlijenata(idKlijenta);
    pthread_mutex_lock(&monitorZaBrijaca);
    otvorenSalon = 0;
    pthread_mutex_unlock(&monitorZaBrijaca);
    pthread_exit(NULL);
}

void *dretvaKlijent(void *arg) {
    int idKlijenta = *((int *)arg); 
    pthread_mutex_lock(&monitorZaBrijaca);
    printf("        Klijent(%d): Zelim na brijanje\n", idKlijenta);
    if (otvorenSalon && slobodnihMjesta < maksimalanBrojKlijenata) {
        slobodnihMjesta++;
        printf("        Klijent(%d): Ulazim u cekaonicu. (%d)\n", idKlijenta, slobodnihMjesta);
        pthread_mutex_unlock(&monitorZaBrijaca);
        usleep(rand() % 1000000);
        pthread_mutex_lock(&monitorZaBrijaca);
        pthread_mutex_unlock(&monitorZaBrijaca);
        usleep(rand() % 1000000);
    } else {
        printf("        Klijent(%d): Salon je zatvoren ili nema slobodnih mjesta. Izlazim.\n", idKlijenta);
        pthread_mutex_unlock(&monitorZaBrijaca);
    }
    pthread_exit(NULL);
}

void *dretvaBrijac(void *arg) {
    while (1) {
        pthread_mutex_lock(&monitorZaBrijaca);

        if (!otvorenSalon && slobodnihMjesta == 0) {
            printf("Brico: Zatvaram salon.\n");
            pthread_mutex_unlock(&monitorZaBrijaca);
            break;
        }

        if(slobodnihMjesta==0){
            printf("Brico: Spavam dok klijenti ne dodju\n");
        }

        if (slobodnihMjesta > 0) {
            printf("Brico: Idem raditi\n");
            slobodnihMjesta--;
            zadovoljniKlijenti++;
            printf("        Klijent(%d): Brico me brije\n", zadovoljniKlijenti);
            pthread_mutex_unlock(&monitorZaBrijaca);

            usleep(rand() % 1000000);

            printf("Brico: Klijent gotov\n");
        } else {
            printf("Brico: Spavam dok klijenti ne dodju\n");
            pthread_mutex_unlock(&monitorZaBrijaca);

            usleep(1000000);
        }
    }
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    printf("Brijac: Otvaram brijacnicu\n");

    KreiranjeZavrsavanjeDretvi();

    printf("\n------------------------------------------------\n");
    printf("Broj ljudi koji su obrijani: %d\n", zadovoljniKlijenti);
    printf("\n------------------------------------------------\n");
    return 0;
}