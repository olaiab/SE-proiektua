#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "processgen.c"
#include "extern_var.c"
#include "pgtimer.c"
#include "sctimer.c"
#include "clock.c"
#include "scheduler.c"


#include "globalak.h"

int main(){
    pthread_t clock_thread, timer_thread, processgen_thread, scheduler_thread;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&cond2, NULL);

    //Mutex
    //Haria, -, errutina, argumentuak
    pthread_create(&clock_thread, NULL, clock_routine, NULL);
    pthread_create(&timer_thread, NULL, pgtimer_routine, NULL);
    pthread_create(&timer_thread, NULL, sctimer_routine, NULL);
    pthread_create(&processgen_thread, NULL, processgen, NULL);
    sleep(10);

    pthread_join(clock_thread, NULL);
    pthread_join(timer_thread, NULL);    
}