#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "globalak.h"

pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_cond_t cond2;

int sc;
int done;
int sctick;
void *sctimer_routine(void *argv){

    int i=0;
    //Mutexa blokeatu
    pthread_mutex_lock(&mutex);
    while(1)
    {
        done++;
         if (i>=erlojua){
            sctick=1;
            i=0;
            //printf("[SCTim] Hola Don José\n");
            //Aqui es donde se pone lo que queramos hacer hasta liberar el mutex
                //Schedulerra martxan
            scheduler();
        }else i++;
        pthread_cond_signal(&cond); //"activa al menos un subproceso que actualmente está esperando en la variable de condición especificada por cond"
        pthread_cond_wait(&cond2, &mutex); //"bloquea el subproceso que realiza la llamada en la variable de condición cond y desbloquea el mutex asociado mutex"
        
    }
}