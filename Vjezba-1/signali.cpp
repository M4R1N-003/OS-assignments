#include <iostream>
#include <signal.h>
#include <time.h>
using namespace std;

const int N = 8;
int oznakaCekanja[N];
int prioritet[N];
int tekuciPrioritet;

bool cekanje=false;

void obrada_prekida(int j){
    time_t vrijemeTr = time(NULL);
    time_t  vrijemePo = time(NULL);
    cout << "Pocela obrada prekida: " << j << endl;
    int k=0;

    while(true){
        time_t novoVrijeme = time(NULL);
        if(novoVrijeme- vrijemeTr != 0 && k!=5){
            k++;
            cout << "Obrada prekida: " << j << "   " << k << "/5" << endl;
            vrijemeTr = novoVrijeme;
        }
        else if(k>=5){
            break;
        }
    }
    cout << "Zavrsila obrada prekida: " << j << endl;
}

void prekidnaRutina(int brSig){
    cekanje=true;
    int i;

    time_t vrijeme; 

    time(&vrijeme);
    
    cout << "\nPrekidna rutina pozvana u: " << ctime(&vrijeme) << endl;
    cout << "Unesi razinu prekida: ";
    cin >> i;
    cekanje=false;

    oznakaCekanja[i] = 1;
    int x;
    do{
        x = 0;
        for(int j=tekuciPrioritet+1;j<N;j++){
            if(oznakaCekanja[j] != 0){
                x=j;
            }
        }

        if(x > 0){
            oznakaCekanja[x] = 0;
            prioritet[x] = tekuciPrioritet;
            tekuciPrioritet = x;
            sigrelse(SIGINT);
            obrada_prekida(x);
            sighold(SIGINT);
            tekuciPrioritet = prioritet[x];

        }
        
    }while(x > 0);
}

int main(){

    time_t vrijemeTr = time(NULL);
    time_t  vrijemePo = time(NULL);

    sigset(SIGINT, prekidnaRutina);
    cout << "Poceo je osnovni program!" << endl;
    int k=0;

    while(true){
        if(cekanje==false){
            time_t novoVrijeme = time(NULL);
            if(novoVrijeme- vrijemeTr != 0 && k!=10){
                k++;
                cout << "Proslo je: " << k << "/10" << endl;
                vrijemeTr = novoVrijeme;
            }
            else if(k>=10){
                break;
            }
        }
        else{
            vrijemeTr = time(NULL);
        }
    }

    cout << "Zavrsio osnovni program!" << endl;
    return 0;
}