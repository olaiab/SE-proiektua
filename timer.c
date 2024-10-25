#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "globalak.h"

void *timer_routine(void *argv){
    printf("Timer\n");
    pthread_mutex_lock(&mutex);
    while(1){
        printf("Timer: While(1)\n");
        done++;
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond2, &mutex);
    }
    printf("Timer: done\n");
}