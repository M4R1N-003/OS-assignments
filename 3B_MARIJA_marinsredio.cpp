#include <sys/sem.h>
#include <cstdlib>
#include <sys/wait.h>
#include <signal.h>
#include <sys/shm.h>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <semaphore.h>

using namespace std;

struct shareVarijable{
        int uuulaz, iiiizlaz, poooljeM[5];
        int ukupnaVrijednost;
};

int siframarija;
key_t tinaValen = IPC_RMID;

void operacija(int num,int op){
	struct sembuf slogSemafora;
    slogSemafora.sem_num=num;
    slogSemafora.sem_op=op;
    slogSemafora.sem_flg=0;
    semop(siframarija, &slogSemafora,1);
}

int postavljanje(int prvi, int drugi) {
        return semctl(siframarija, prvi, SETVAL, drugi);
}

shareVarijable *shmVarijable;
int brojBrojcica;

void proizvodac(int ar){
        for(int i = 0; i < brojBrojcica; i++){
        
        operacija(1,-1);
        operacija(0,-1);
        srand(time(0));
        sleep(1);
        int randombroj = rand()%1000;
        shmVarijable-> poooljeM[shmVarijable -> uuulaz]=randombroj;
        cout << "Proizvodac " << ar+1<<" salje \""<<shmVarijable ->poooljeM[shmVarijable-> uuulaz]<<"\""<<endl;
        shmVarijable->uuulaz = (shmVarijable -> uuulaz+1)%5;

        operacija(0,1);
        operacija(2,1);

    }
    cout << "Proizvodac "<< ar+1<<" je zavrsio sa slanjem" << endl;
}

int IDprocesa,brojProcesica;

void potrosac(){
    int sumazbroj=0;
    int h=0;

    int primljenbroj;
    while(h < shmVarijable->ukupnaVrijednost){
        operacija(2,-1);
        sleep(1);
        primljenbroj =shmVarijable->poooljeM[shmVarijable->iiiizlaz];
        cout << "Potrosac prima " << primljenbroj<<endl;
        sumazbroj=sumazbroj+primljenbroj;
        shmVarijable->iiiizlaz = (shmVarijable->iiiizlaz+1)%5;
        operacija(1,1);
        h++;
    }
    cout  <<"Potrosac - zbroj primljenih brojeva="<<sumazbroj<<endl;
}

void ubijanjeProcesa(int brojic){

    shmdt(shmVarijable);
    shmctl(IDprocesa,tinaValen,NULL);
    semctl(siframarija, 0, tinaValen, 0);
    exit(1);
}

void postavljanjeSemafora(){
	postavljanje(0,1);
    postavljanje(1,5);
    postavljanje(2,0);	
}

void dohvacanjeMemorije(){
	IDprocesa = shmget(IPC_PRIVATE, sizeof(shareVarijable), 0600);
    siframarija=semget(IPC_PRIVATE,3,0600);
    shmVarijable=(shareVarijable *)shmat(IDprocesa,NULL,0);
}

void kreiranjeProcesa(){
	for(int z=0; z<brojProcesica; z++){
        if(fork()==0){
            proizvodac(z);
            exit(0);
        }
    }
}

void kreiranjePotrosaca(){
	if(fork()==0){
        potrosac();
        exit(0);
    }
}

void waitanjeProcesa(){
	for(int c=0; c<brojProcesica+1; c++){
        wait(NULL);
    }
}
int main(int argc, char*argv[]){
    
    brojProcesica=stoi(argv[1]);
    brojBrojcica=stoi(argv[2]);
    dohvacanjeMemorije();
    sigset(SIGINT,ubijanjeProcesa);
    
    shmVarijable->ukupnaVrijednost=brojBrojcica*brojProcesica;
    shmVarijable -> uuulaz =  0;
	shmVarijable-> iiiizlaz = 0;
	
	postavljanjeSemafora();

    kreiranjeProcesa();
    
    kreiranjePotrosaca();
    
    waitanjeProcesa();

    shmdt(shmVarijable);
    shmctl(IDprocesa,tinaValen,NULL);
    semctl(siframarija, 0, tinaValen, 0);
    exit(1);

}