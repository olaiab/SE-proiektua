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
int CPUk;
int corek;
int threadsk;
int quantum;
int i;
int j;
int k;
int blocked;
int politika;

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
        printf("Erlojuaren maiztasuna ezarri:   ");
        while (!sarrera){
            scanf("%d", &input);
            if (input < 1){
                printf("\nSarrera okerra, sartu berriro:   ");
            } else sarrera++;
        }
        erlojua=input;
        //CPU kopurua
        sarrera=0;
        input=-666;
        printf("\nCPU kopurua ezarri (1, 2, 4, 8, 16, 32, 64):   ");
        while (!sarrera){
            scanf("%d", &input);
            if (input != 1 && input != 2 && input != 4 && input != 8 && input != 16 && input != 32 && input != 64 ){
                printf("\nSarrera okerra, sartu berriro:   ");
            } else sarrera++;
        }
        CPUk=input;
        //Core 
        sarrera=0;
        input=-666;
        printf("\nCore kopurua ezarri (1, 2, 4, 8, 16, 32, 64):   ");
        while (!sarrera){
            scanf("%d", &input);
            if (input != 1 && input != 2 && input != 4 && input != 8 && input != 16 && input != 32 && input != 64 ){
                printf("\nSarrera okerra, sartu berriro:   ");
            } else sarrera++;
        }
        corek=input;
        //Hari kopurua
        sarrera=0;
        input=-666;
        printf("\nHari kopurua ezarri (1, 2, 4, 8):   ");
        while (!sarrera){
            scanf("%d", &input);
            if (input != 1 && input != 2 && input != 4 && input != 8){
                printf("\nSarrera okerra, sartu berriro:   ");
            } else sarrera++;
        }
        threadsk=input;
        //Planifikazio politika
        sarrera=0;
        input=-666;
        char *politikak[]={
            "RoundRobin",
            "First come, first served",
            "Lehentasunak"
        };

        printf("\nPlanifikazio politika zehaztu:\n");
        printf("RoundRobin -------------------------- 1\n");
        printf("First come, first served ------------ 2\n");
        printf("Lehentasunak ------------------------ 3\n\n");
        while (!sarrera){
            scanf("%d", &input);
            if (input != 1 && input != 2 && input != 3){
                printf("\nSarrera okerra, sartu berriro:    ");
            } else sarrera++;
        }
        politika=input;
        //Quantuma
        if (politika==1){
            sarrera=0;
            input=-666;
            printf("\nQuantumaren balioa ezarri:    ");
            while (!sarrera){
                scanf("%d", &input);
                if (input < 1){
                    printf("\nSarrera okerra, sartu berriro:  ");
                } else sarrera++;
            }
            quantum=input;
        } else quantum=-1;
        

    //Hardwarea hasieratu
    CPU_list = malloc(sizeof(struct CPU) * (int)CPUk);
    for (i=0; i<CPUk; i++){
        CPU_list[i].core_list=malloc(sizeof (struct core) * (int)corek);
        for (j=0; j<corek; j++){
            CPU_list[i].core_list[j].thread_list=malloc(sizeof (struct thread) * (int)threadsk);
            for (k=0; k<threadsk; k++){
                CPU_list[i].core_list[j].thread_list[k].pcb.pid=-1;
                CPU_list[i].core_list[j].thread_list[k].quantum=quantum;
                CPU_list[i].core_list[j].thread_list[k].libre=0;
            }
        }
    }
    

    printf("\n-------AUKERATUTAKO KONFIGURAZIOA-------\n");
    printf("CPU kopurua: ------------------------- %d\n",CPUk);
    printf("Core kopurua: ------------------------ %d\n",corek);
    printf("Hari kopurua: ------------------------ %d\n",threadsk);
    printf("Erlojuaren maiztasuna: --------------- %d\n",erlojua);
    printf("Aukeratutako politika: --------------- %s\n",politikak[politika-1]);
    if (politika==1) ("Quantumaren balioa: ------------------ %d\n",quantum);
    printf("\n");


    //Mutex
    //Haria, -, errutina, argumentuak
    pthread_create(&clock_thread, NULL, clock_routine, NULL);
    pthread_create(&pgtimer_thread, NULL, pgtimer_routine, NULL);
    pthread_create(&sctimer_thread, NULL, sctimer_routine, NULL);
    //Prozesu sortzailea
    pthread_create(&processgen_thread, NULL, processgen, NULL);
    //Scheduler
    if (politika==1)        pthread_create(&scheduler_thread, NULL, roundRobin, NULL);
    else if (politika==2)   pthread_create(&scheduler_thread, NULL, fcfs, NULL);
    sleep(10);

    pthread_join(clock_thread, NULL);
    pthread_join(pgtimer_thread, NULL);
    pthread_join(sctimer_thread, NULL);
    pthread_join(processgen_thread, NULL);   
    pthread_join(scheduler_thread, NULL);
}