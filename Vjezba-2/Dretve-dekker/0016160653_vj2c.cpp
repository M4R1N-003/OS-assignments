#include <iostream>
#include <unistd.h>
#include <thread>

using namespace std;

const int n = 2;
const int sveukupnoPonavljanja = 5;
const int sveukupnoSekcija = 5;

int PRAVO = 0;
bool ZASTAVICA[2] = {false, false};
int brojac[2] = {0, 0};

void udji_u_kriticni_odsjecak(int i, int j) {
    ZASTAVICA[i] = true;
    while (ZASTAVICA[j]) {
        if (PRAVO == j) {
            ZASTAVICA[i] = false;
            while (PRAVO == j) {
            }
            ZASTAVICA[i] = true;
        }
    }
}

void izadji_iz_kriticnog_odsjecka(int i) {
    PRAVO = 1 - i;
    ZASTAVICA[i] = false;
}

void proc(int i) {
    for (int k = 1; k <= sveukupnoSekcija; k++) {
        udji_u_kriticni_odsjecak(i, 1 - i);
        for (int m = 1; m <= sveukupnoPonavljanja; m++) {
            brojac[i]++;
            cout << "Dretva: " << i + 1 << ", K.O. br: " << k << " (" << brojac[i] << "/" << sveukupnoPonavljanja << ")" << endl;
            sleep(1);
        }
        izadji_iz_kriticnog_odsjecka(i);
        brojac[i] = 0;
    }
}

int main() {
    thread t[n];

    for (int i = 0; i < n; i++) {
        t[i] = thread(proc, i);
    }

    for (int i = 0; i < n; i++) {
        t[i].join();
    }

    return 0;
}
