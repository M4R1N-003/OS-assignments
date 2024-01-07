#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>
#include<ctime>
#include<unistd.h>

const int brojDva = 2;

int desna_obala = 1;
int trenutni_broj_u_cunu = 0;
int kojiPutniciSuUcunu[brojDva] = {0};

sem_t semaforcek_cun, semafori_reguliranja[brojDva][brojDva], kriticni_odsjecak_semaforcek, ukrcati_putnike_sem, iskrcati_putnike_sem;

int sumaOsoba[brojDva][brojDva] = {0};

//Dretva koja opisuje ponasanje kanibala i njihovo ukrcavanje na cun
void *dretvaStvaranjaKanibala(void *obalaArg){
    int obalica_na_kojoj_smo = *((int*)obalaArg);
    sem_wait(&kriticni_odsjecak_semaforcek);

    sumaOsoba[obalica_na_kojoj_smo][0]++;
    int tinaValen = 0;
    while(!tinaValen){
        if(desna_obala == obalica_na_kojoj_smo && trenutni_broj_u_cunu < 7 && kojiPutniciSuUcunu[1] > kojiPutniciSuUcunu[0]){
            tinaValen = 1;
            sumaOsoba[obalica_na_kojoj_smo][0]--;
            trenutni_broj_u_cunu++;
            kojiPutniciSuUcunu[0]++;
            if(trenutni_broj_u_cunu == 3){
               sem_post(&ukrcati_putnike_sem);
            }
            if(trenutni_broj_u_cunu >= 3 && trenutni_broj_u_cunu < 7 && sumaOsoba[obalica_na_kojoj_smo][1] > 0){
                sem_post(&semafori_reguliranja[obalica_na_kojoj_smo][1]);
            }
        }
        else{
            sem_post(&kriticni_odsjecak_semaforcek);
            sem_wait(&semafori_reguliranja[obalica_na_kojoj_smo][0]);
            sem_wait(&kriticni_odsjecak_semaforcek);
        }
    }
    sem_post(&kriticni_odsjecak_semaforcek);
    sem_wait(&semaforcek_cun);
    sem_wait(&kriticni_odsjecak_semaforcek);
    printf("LJUDOZDER, ");
    trenutni_broj_u_cunu--;
    kojiPutniciSuUcunu[0]--;
    if(trenutni_broj_u_cunu == 0) sem_post(&iskrcati_putnike_sem);
    sem_post(&kriticni_odsjecak_semaforcek);
    return NULL;
}

//Inicializacija semafora
void postavljanjePocetneVrijednosti(){
    for(int brojac = 0; brojac < brojDva; brojac++)
        for(int brojac2 = 0; brojac2 < brojDva; brojac2++)
            sem_init(&semafori_reguliranja[brojac][brojac2], 0, 0);
    sem_init(&semaforcek_cun, 0, 0);
    sem_init(&kriticni_odsjecak_semaforcek, 0, 1);
    sem_init(&ukrcati_putnike_sem, 0, 0);
    sem_init(&iskrcati_putnike_sem, 0, 0);
}

//Ispis prijevoza putnika preko obale
void ispisivanje(){
    printf("Prevezeni na drugu obalu: ");
    desna_obala = 1 - desna_obala;
    sleep(brojDva);
    for(int brojac = 0; brojac < trenutni_broj_u_cunu; brojac++)
        sem_post(&semaforcek_cun);
    sem_wait(&iskrcati_putnike_sem);
    printf("\n");
}

//Ukrcavanje, prijevoz i iskrcavanje putnika preko rijeke
void *cunDretvica(void *soba207){
    desna_obala = 1;
    do{
        sem_wait(&ukrcati_putnike_sem);
        sleep(1);
        ispisivanje();
        for(int brojac = 0; brojac < sumaOsoba[desna_obala][1]; brojac++) sem_post(&semafori_reguliranja[desna_obala][1]);
        for(int brojac = 0; brojac < sumaOsoba[desna_obala][0]; brojac++) sem_post(&semafori_reguliranja[desna_obala][0]);
    }while(89);
}

//Dretva koja opisuje ponasanje misionara i njihovo ukrcavanje na cun
void *dretvaStvaranjaMisionara(void *obalaArg){
    int obalica_na_kojoj_smo = *((int*)obalaArg);
    sem_wait(&kriticni_odsjecak_semaforcek);
    sumaOsoba[obalica_na_kojoj_smo][1]++;
    int tinaValen = 0;
    while(!tinaValen){
        if(desna_obala == obalica_na_kojoj_smo && trenutni_broj_u_cunu < 7 && kojiPutniciSuUcunu[1] >= kojiPutniciSuUcunu[0])
        {
            tinaValen = 1;
            sumaOsoba[obalica_na_kojoj_smo][1]--;
            trenutni_broj_u_cunu++;
            kojiPutniciSuUcunu[1]++;
            if(trenutni_broj_u_cunu == 3){
                sem_post(&ukrcati_putnike_sem);
            }
            if(trenutni_broj_u_cunu >= 3 && trenutni_broj_u_cunu < 7 && sumaOsoba[obalica_na_kojoj_smo][0] > 0){
                sem_post(&semafori_reguliranja[obalica_na_kojoj_smo][0]);
            }
        }
        else{
            sem_post(&kriticni_odsjecak_semaforcek);
            sem_wait(&semafori_reguliranja[obalica_na_kojoj_smo][1]);
            sem_wait(&kriticni_odsjecak_semaforcek);
        }
    }
    sem_post(&kriticni_odsjecak_semaforcek);
    sem_wait(&semaforcek_cun);
    sem_wait(&kriticni_odsjecak_semaforcek);
    printf("MISIONAR, ");
    trenutni_broj_u_cunu--;
    kojiPutniciSuUcunu[1]--;
    if(trenutni_broj_u_cunu == 0) sem_post(&iskrcati_putnike_sem);
    sem_post(&kriticni_odsjecak_semaforcek);
    return NULL;
}

//Generiranje svih osoba/dretava
void *dretvaSumica(void *soba207){
    pthread_t identifikacijskiBroj;
    do{
        int obalica_na_kojoj_smo = rand()%brojDva;
        pthread_create(&identifikacijskiBroj,NULL,dretvaStvaranjaKanibala,&obalica_na_kojoj_smo);
        sleep(1);
        obalica_na_kojoj_smo = rand()%brojDva;
        pthread_create(&identifikacijskiBroj,NULL,dretvaStvaranjaKanibala,&obalica_na_kojoj_smo);
        obalica_na_kojoj_smo = rand()%brojDva;
        pthread_create(&identifikacijskiBroj,NULL,dretvaStvaranjaMisionara,&obalica_na_kojoj_smo);
        sleep(1);
    }while(89);
    return NULL;
}

//Unistavanje svih semafora
void unistiti(int valnt){
    switch(valnt){
        case 1:
            for(int brojac = 0; brojac < brojDva; brojac++){
                for(int brojac2 = 0; brojac2 < brojDva; brojac2++)
                    sem_destroy(&semafori_reguliranja[brojac][brojac2]);
            }
        break;
        case 2:
            sem_destroy(&ukrcati_putnike_sem);
        break;
        case 3:
            sem_destroy(&iskrcati_putnike_sem);
        break;
        case 4:
            sem_destroy(&semaforcek_cun);
        break;
        case 5:
            sem_destroy(&kriticni_odsjecak_semaforcek);
        break;
    }
}

//Kreiranje dretve cun i pozivanje funkcije dretvaSumica koja zapocinje paralelno izvodjenje programa
void pokretanjePrograma(){
    postavljanjePocetneVrijednosti();
    pthread_t identifikacijskiBroj;
    pthread_create(&identifikacijskiBroj, NULL, cunDretvica, 0);
    dretvaSumica(0);
}

int main(){
    srand(time(0));

    //Pokrece se
    pokretanjePrograma();

    //Unistavanje semafora
    unistiti(1);
    unistiti(2);
    unistiti(3);
    unistiti(4);
    unistiti(5);
    return 0;
}