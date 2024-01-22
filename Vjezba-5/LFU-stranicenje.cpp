#include <iostream>
using namespace std;

const int MAX_FRAMES = 4;

class LFUCache {
private:
    int velicina;
    int time;
    int trenutnaVelicina = 0;
    int* stranice;
    int* ponavljanja;
    int* posljednjiPutPristupljeno;

public:
    LFUCache(int velicina) : velicina(velicina), time(0) {
        stranice = new int[velicina];
        ponavljanja = new int[velicina];
        posljednjiPutPristupljeno = new int[velicina];
        for (int i = 0; i < velicina; ++i) {
            stranice[i] = -1;
            ponavljanja[i] = 0;
            posljednjiPutPristupljeno[i] = 0;
        }
    }

    ~LFUCache() {
        delete[] stranice;
        delete[] ponavljanja;
        delete[] posljednjiPutPristupljeno;
    }


    int pronadjiStranicuUslotovima(int trenutnaStranica) {
        for (int i = 0; i < trenutnaVelicina; ++i) {
            if (stranice[i] == trenutnaStranica) {
                return i; 
            }
        }
        return -1;  
    }


    int pronadjiStranicukojasenajmanjeponavlja() {
        int minIndex = 0;
        for (int i = 1; i < trenutnaVelicina; ++i) {
            if (ponavljanja[i] < ponavljanja[minIndex] ||
                (ponavljanja[i] == ponavljanja[minIndex] && posljednjiPutPristupljeno[i] < posljednjiPutPristupljeno[minIndex])) {
                minIndex = i;
            }
        }
        return stranice[minIndex];
    }

    void printCache() {
        cout << "#N 1 2 3 4 (n,t) (n,t) (n,t) (n,t)\n";
        cout << "---------------------------------------------\n";
        for (int i = 0; i < trenutnaVelicina; ++i) {
            cout << stranice[i] << " ";
            for (int j = 0; j < MAX_FRAMES; ++j) {
                if (j < trenutnaVelicina) {
                    if (stranice[j] == stranice[i]) {
                        cout << "[" << stranice[j] << "]";
                    } else {
                        cout << " - ";
                    }
                } else {
                    cout << " - ";
                }
            }
            cout << "(" << ponavljanja[i] << "," << posljednjiPutPristupljeno[i] << ") ";
            cout << endl;
        }
        cout << endl;
    }
 
    void pristupiStranici(int trenutnaStranica) {
        time++;

        int indeksSlota = pronadjiStranicuUslotovima(trenutnaStranica);

        if (indeksSlota != -1) {
            ponavljanja[indeksSlota]++;
            posljednjiPutPristupljeno[indeksSlota] = time;
        } else {
            if (trenutnaVelicina == velicina) {
                int minPage = pronadjiStranicukojasenajmanjeponavlja();
                indeksSlota = pronadjiStranicuUslotovima(minPage);

                stranice[indeksSlota] = trenutnaStranica;
                ponavljanja[indeksSlota] = 1;
                posljednjiPutPristupljeno[indeksSlota] = time;
            } else {
                stranice[trenutnaVelicina] = trenutnaStranica;
                ponavljanja[trenutnaVelicina] = 1;
                posljednjiPutPristupljeno[trenutnaVelicina] = time;
                trenutnaVelicina++;
            }
        }

        printCache();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Nevaljali broj argumenata!\n";
        return 1;
    }

    int velicina = stoi(argv[1]);
    int numRequests = stoi(argv[2]);

    LFUCache lfuCache(velicina);

    cout << "Zahtjevi: ";
    for (int i = 0; i < numRequests; ++i) {
        int pageRequest;
        cin >> pageRequest;
        lfuCache.pristupiStranici(pageRequest);
        cout << pageRequest << ",";
    }

    return 0;
}
