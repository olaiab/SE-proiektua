#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "globalak.h"

pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_cond_t cond2;

int done;
int tenp_kop;
void *clock_routine(void *argv){
    //Semaforoa hasieratu
    done=0;
    tenp_kop=2;
    while(1){
        pthread_mutex_lock(&mutex);
        while (done < tenp_kop){
            //Mutexa liberatu cond seinalea jaso arte
            pthread_cond_wait(&cond, &mutex);
        }
        //printf("\n\033[1;33m[Clock] Tick\033[0m\n");
        done=0;
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}