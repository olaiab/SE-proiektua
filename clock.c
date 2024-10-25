#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "globalak.h"

void *clock_routine(void *argv){
    //Semaforoa hasieratu
    done=0;
    tenp_kop=1;
    printf("Clock \n");
    while(1){
        printf("Clock: While(1)\n");
        pthread_mutex_lock(&mutex);
        while (done < tenp_kop){
            printf("Clock: While done\n");
            pthread_cond_wait(&cond, &mutex);
        }
        sleep(1);
        done=0;
        printf("Clock: Done\n");
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}