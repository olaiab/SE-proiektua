#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "globalak.h"

void *timer_routine(void *argv){
    //Mutexa blokeatu
    pthread_mutex_lock(&mutex);
    while(1){
        printf("[Timer] Hola Don José\n");
        //Aqui es donde se pone lo que queramos hacer hasta liberar el mutex
        done++;
        pthread_cond_signal(&cond); //"activa al menos un subproceso que actualmente está esperando en la variable de condición especificada por cond"
        pthread_cond_wait(&cond2, &mutex); //"bloquea el subproceso que realiza la llamada en la variable de condición cond y desbloquea el mutex asociado mutex"
        
    }
}