#include <iostream>
#include <unistd.h>
#include <pthread.h>

using namespace std;

const int n = 2;
const int sveukupnoPonavljanja = 5;
const int sveukupnoSekcija = 5;

int PRAVO = 0;
bool ZASTAVICA[2] = {false, false};
int brojac[2] = {0, 0};
bool mojRed[n] = {false, false};

void udji_u_kriticni_odsječak(int i) {
    mojRed[i] = true;
    while (mojRed[1 - i]) {
        if (PRAVO == 1 - i) {
            mojRed[i] = false;
            while (PRAVO == 1 - i) {
            }
            mojRed[i] = true;
        }
    }
}

void izadji_iz_kriticnog_odsjecka(int i) {
    PRAVO = 1 - i;
    mojRed[i] = false;
}

void* obradi(void* arg) {
    int i = *(int*)arg;
    for (int k = 1; k <= sveukupnoSekcija; k++) {
        udji_u_kriticni_odsječak(i);
        for (int m = 1; m <= sveukupnoPonavljanja; m++) {
            brojac[i]++;
            cout << "\n" << "Dretva: " << i + 1 << ", K.O. br: " << k << " (" << brojac[i] << "/" << sveukupnoPonavljanja << ")";
            sleep(1);
        }
        izadji_iz_kriticnog_odsjecka(i);
        brojac[i] = 0;
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t dretve[n];
    int argumenti_dretvi[n];

    for (int i = 0; i < n; i++) {
        argumenti_dretvi[i] = i;
        int rezultat = pthread_create(&dretve[i], NULL, obradi, &argumenti_dretvi[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(dretve[i], NULL);
    }

    return 0;
}
