#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "processgen.c"
#include "pgtimer.c"
#include "sctimer.c"
#include "clock.c"
#include "scheduler.c"

#include "globalak.h"

pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_cond_t cond2;

pthread_t clock_thread, pgtimer_thread, sctimer_thread, processgen_thread, scheduler_thread;

int erlojua;
int CPU;
int core;
int threads;
int quantum;

struct CPU *CPU_list;
int main(){
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&cond2, NULL);

    //Konfigurazio parametroak jaso
    int sarrera;
    int input=-666;
        //Erlojuaren maiztasuna
        sarrera=0;
        input=-666;
        printf("Erlojuaren maiztasuna ezarri:\n");
        while (!sarrera){
            scanf("%d", &input);
            if (input < 1){
                printf("Sarrera okerra, sartu berriro:\n");
            } else sarrera++;
        }
        erlojua=input;
        //CPU kopurua
        sarrera=0;
        input=-666;
        printf("CPU kopurua ezarri (1, 2, 4, 8, 16, 32, 64):\n");
        while (!sarrera){
            scanf("%d", &input);
            if (input != 1 && input != 2 && input != 4 && input != 8 && input != 16 && input != 32 && input != 64 ){
                printf("Sarrera okerra, sartu berriro:\n");
            } else sarrera++;
        }
        CPU=input;
        //Core 
        sarrera=0;
        input=-666;
        printf("Core kopurua ezarri (1, 2, 4, 8, 16, 32, 64):\n");
        while (!sarrera){
            scanf("%d", &input);
            if (input != 1 && input != 2 && input != 4 && input != 8 && input != 16 && input != 32 && input != 64 ){
                printf("Sarrera okerra, sartu berriro:\n");
            } else sarrera++;
        }
        core=input;
        //Hari kopurua
        sarrera=0;
        input=-666;
        printf("Hari kopurua ezarri (1, 2, 4, 8):\n");
        while (!sarrera){
            scanf("%d", &input);
            if (input != 1 && input != 2 && input != 4 && input != 8){
                printf("Sarrera okerra, sartu berriro:\n");
            } else sarrera++;
        }
        thread=input;
        //Quantum
        sarrera=0;
        input=-666;
        printf("Quantumaren balioa ezarri:\n");
        while (!sarrera){
            scanf("%d", &input);
            if (input < 1){
                printf("Sarrera okerra, sartu berriro:\n");
            } else sarrera++;
        }
        quantum=input;

    //Hardwarea hasieratu
    
    
    //Mutex
    //Haria, -, errutina, argumentuak
    pthread_create(&clock_thread, NULL, clock_routine, NULL);
    pthread_create(&pgtimer_thread, NULL, pgtimer_routine, NULL);
    pthread_create(&sctimer_thread, NULL, sctimer_routine, NULL);
    //Prozesu sortzailea
    pthread_create(&processgen_thread, NULL, processgen, NULL);
    sleep(10);

    pthread_join(clock_thread, NULL);
    pthread_join(pgtimer_thread, NULL);
    pthread_join(sctimer_thread, NULL);
    pthread_join(processgen_thread, NULL);        
}