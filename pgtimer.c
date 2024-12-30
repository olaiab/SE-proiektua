#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "globalak.h"

void *pgtimer_routine(void *argv){
    pg=0;
    //Mutexa blokeatu
    pthread_mutex_lock(&mutex);
    while(1){
        printf("[PGTim] Hola Don Pepito\n");
        //Aqui es donde se pone lo que queramos hacer hasta liberar el mutex
            //darle permiso a processgen para existir
        pg=1;
        done++;
        pthread_cond_signal(&cond); //"activa al menos un subproceso que actualmente está esperando en la variable de condición especificada por cond"
        pthread_cond_wait(&cond2, &mutex); //"bloquea el subproceso que realiza la llamada en la variable de condición cond y desbloquea el mutex asociado mutex"
        
    }
}