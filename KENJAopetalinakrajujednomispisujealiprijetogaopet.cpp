#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

#define MAX_MJESTA 5
#define BROJ_KLIJENATA 10
#define RADNO_VRIJEME 3

pthread_mutex_t monitor = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t red_brijaca = PTHREAD_COND_INITIALIZER;
bool otvoreno = true;
int br_mjesta = 0;
bool brijac_slobodan = true;
int zavrseni_klijenti = 1;
pthread_t brijac;
pthread_t generator;
bool zatvori_brijacnicu = false;
bool radno_vrijeme_isteklo = false;
bool generator_zavrsio = false;

void* dretva_brijac(void*) {
    while (1) {
        pthread_mutex_lock(&monitor);

        // Dodatni uvjet za provjeru radnog vremena i klijenata
        while (br_mjesta == 0 && zavrseni_klijenti <= BROJ_KLIJENATA && !radno_vrijeme_isteklo && !brijac_slobodan && !zatvori_brijacnicu) {
            std::cout << "Brijac: Spim dok klijenti ne dodu" << std::endl;
            pthread_cond_wait(&red_brijaca, &monitor);
        }

        if ((zavrseni_klijenti > BROJ_KLIJENATA && otvoreno) || zatvori_brijacnicu) {
            if (zavrseni_klijenti > BROJ_KLIJENATA && otvoreno) {
                radno_vrijeme_isteklo = true;
            }
            if (!zatvori_brijacnicu) {
                std::cout << "Brijac: Zatvaram brijacnicu" << std::endl;
                zatvori_brijacnicu = true;
            }
            pthread_mutex_unlock(&monitor);
            pthread_exit(NULL);
        }

        // Ostatak koda ostaje nepromijenjen
        brijac_slobodan = false;

        if (br_mjesta > 0) {
            std::cout << "Brijac: Pocinjem raditi" << std::endl;
            pthread_cond_signal(&red_brijaca);
            br_mjesta--;
            usleep(100000);
            std::cout << "Klijent(" << zavrseni_klijenti << "): Brijac me brije" << std::endl;
            std::cout << "Brijac: Klijent gotov" << std::endl;
            zavrseni_klijenti++;

            if (zavrseni_klijenti > BROJ_KLIJENATA) {
                std::cout << "Brijac: Zatvaram brijacnicu." << std::endl;
                zatvori_brijacnicu = true;
            }
        } else if (!otvoreno) {
            std::cout << "Brijac: Zatvaram brijacnicu, nema vise klijenata." << std::endl;
            zatvori_brijacnicu = true;
        }

        brijac_slobodan = true;

        pthread_mutex_unlock(&monitor);

        usleep(rand() % 100000);
    }
    pthread_exit(NULL);
}

void* dretva_klijent(void* arg) {
    int id = *((int*)arg);

    pthread_mutex_lock(&monitor);

    std::cout << "Klijent(" << id << "): Zelim brijanje" << std::endl;

    if (!radno_vrijeme_isteklo && otvoreno && br_mjesta < MAX_MJESTA && brijac_slobodan) {
        br_mjesta++;
        std::cout << "Klijent(" << id << "): Ulazim u cekaonicu. (" << br_mjesta << ")" << std::endl;
        pthread_cond_signal(&red_brijaca);
    } else if (!radno_vrijeme_isteklo && otvoreno) {
        std::cout << "Klijent(" << id << "): Isteklo radno vrijeme ili nema mjesta. Odlazim." << std::endl;
        if (id == BROJ_KLIJENATA) {
            radno_vrijeme_isteklo = true;
        }
    } else {
        std::cout << "Klijent(" << id << "): Isteklo radno vrijeme ili nema mjesta. Odlazim." << std::endl;
    }

    pthread_mutex_unlock(&monitor);

    usleep(rand() % 1000000);

    pthread_exit(NULL);
}

void* generatorklijenata(void*) {
    int id_klijenta = 0;

    while (id_klijenta < BROJ_KLIJENATA) {
        pthread_mutex_lock(&monitor);
        while (br_mjesta >= MAX_MJESTA) {
            pthread_mutex_unlock(&monitor);
            usleep(10000); // Sleep for a short period to avoid busy-waiting
            pthread_mutex_lock(&monitor);
        }
        pthread_mutex_unlock(&monitor);

        pthread_t dretva_za_klijenta;
        pthread_create(&dretva_za_klijenta, NULL, dretva_klijent, (void*)&id_klijenta);
        id_klijenta++;

        usleep(rand() % 300000);

        if (id_klijenta >= BROJ_KLIJENATA) {
            generator_zavrsio = true; // Signal that the generator thread has finished its work
            pthread_cond_broadcast(&red_brijaca); // Broadcast to wake up the barber thread
            break; // Terminate the loop once the maximum number of clients is reached
        }
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));

    std::cout << "Brijac: Otvaram brijacnicu" << std::endl;

    pthread_create(&brijac, NULL, dretva_brijac, NULL);
    pthread_create(&generator, NULL, generatorklijenata, NULL);

    sleep(RADNO_VRIJEME);

    pthread_mutex_lock(&monitor);
    otvoreno = false; // Set the flag to close the barbershop
    pthread_mutex_unlock(&monitor);


    pthread_join(brijac, NULL);
    pthread_join(generator, NULL);

    return 0;
}
