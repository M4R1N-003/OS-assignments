#include <iostream>

using namespace std;

struct slotBuffer {
    int stranica;
    int vrijeme;
};

void printmemorija(slotBuffer* memorija, int brojSlotova) {
    cout << "#N ";
    for (int i = 1; i <= brojSlotova; ++i) {
        cout << i << " ";
    }
    cout << "(n,t) ";
    for (int i = 0; i < brojSlotova; ++i) {
        cout << "(" << memorija[i].stranica << "," << memorija[i].vrijeme << ") ";
    }
    cout << "\n";
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Pogresan broj argumenata!" << endl;
        return 1;
    }

    int brojSlotova = atoi(argv[1]);
    int brojZahtjeva = atoi(argv[2]);

    int* stranicaRequests = new int[brojZahtjeva];

    cout << "Zahtjevi: ";
    for (int i = 0; i < brojZahtjeva; ++i) {
        cin >> stranicaRequests[i];
        cout << stranicaRequests[i] << ((i == brojZahtjeva - 1) ? "\n" : ",");
    }

    slotBuffer* memorija = new slotBuffer[brojSlotova];
    int najstarijaStranica = 0;

    for (int i = 0; i < brojZahtjeva; ++i) {
        int trenutnaStranica = stranicaRequests[i];

        bool stranicaPronadjena = false;
        for (int j = 0; j < brojSlotova; ++j) {
            if (memorija[j].stranica == trenutnaStranica) {
                stranicaPronadjena = true;
                break;
            }
        }

        if (!stranicaPronadjena) {
            if (i < brojSlotova) {
                memorija[i] = {trenutnaStranica, i + 1};
            } else {
                memorija[najstarijaStranica] = {trenutnaStranica, i + 1};
                najstarijaStranica = (najstarijaStranica + 1) % brojSlotova;
            }

            printmemorija(memorija, brojSlotova);
        }
    }

    delete[] stranicaRequests;
    delete[] memorija;

    return 0;
}
