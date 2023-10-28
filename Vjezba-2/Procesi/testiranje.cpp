#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <ctime>
#include <wait.h>

#ifndef N
#define N 30
#endif
using namespace std;
int M;

int* sharePolje;
int* indeks;

void generiranje(int pid, int id){
   cout << "Proces "  << pid << " kreiran za generiranje" << endl;
   srand(time(0));
   for(int i=(id*N)-N-1;i < N; i++){
      sharePolje[i] = rand()%100;
      cout << sharePolje[i] << endl;
   }
}

void ispisivanje(int pid, int id){
   cout << "Proces " << pid << " kreiran za racunanje" << endl;
   int zbroj = 0;
   for(int i=(id*N)-N-1;i < N; i++){
      if(sharePolje[i]!=-1)
         zbroj=zbroj + sharePolje[i];
   }
   cout << "Zbroj procesa " << id << " je: " << zbroj << endl;
   double prosjek;
   prosjek = zbroj/N;
   cout << "Prosjek procesa " << id << " je: " << prosjek << endl;

}
int main(int argc, char *argv[]){
   M = atoi(argv[1]);

   int shmid = shmget(IPC_PRIVATE, 30000, 0600);
   sharePolje = (int*)shmat(shmid, NULL, 0);

   int shmidIndex = shmget(IPC_PRIVATE, 1000, 0600);
   indeks = (int*)shmat(shmidIndex, NULL, 0);
   *indeks = 0;

   for(int i=0;i<sizeof(sharePolje)/sizeof(int);i++){
      sharePolje[i] = -1;
   }

   for(int i=0;i<M*2;i++){
      switch(fork()){
         case -1:
            cout << "Greska sa procesom!" << endl;
         break;
         case 0:
            if(i%2==0){
               (*indeks)++;
               int pid = getpid();
               generiranje(pid, *indeks);
               exit(0);
            }
            else{
               int pid = getpid();
               ispisivanje(pid, *indeks);
               exit(0);
            }
         break;
         default:
         break;
      }
   }

   for(int i=0;i<M*2;i++)
      wait(NULL);

   shmdt(sharePolje);
   shmctl(shmid, IPC_PRIVATE, NULL);

   shmdt((int *)indeks);
   shmctl(shmidIndex, IPC_PRIVATE, NULL);
    return 0;
}