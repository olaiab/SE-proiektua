#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "globalak.h"

void *clock_routine(void *argv){
    //Semaforoa hasieratu
    done=0;
    tenp_kop=2;
    while(1){
        pthread_mutex_lock(&mutex);
        while (done < tenp_kop){
            printf("[Clock] Buenas buenas \n");
            //Mutexa liberatu cond seinalea jaso arte
            pthread_cond_wait(&cond, &mutex);
        }
        sleep(1);
        done=0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}