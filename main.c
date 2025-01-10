#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
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
int pos=0;
int pid=1;
int waiting;

struct CPU *CPU_list;

int main(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&cond2, NULL);

    power=1;
    //programentzako direktorioa
    
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
            while(getchar() != '\n');
        } else sarrera++;
    }
    erlojua=input;
    //CPU kopurua
    sarrera=0;
    input=-666;
    printf("\nCPU kopurua ezarri (1, 2, 4, 8, 16):   ");
    while (!sarrera){
        scanf("%d", &input);
        if (input != 1 && input != 2 && input != 4 && input != 8 && input != 16){
            printf("\nSarrera okerra, sartu berriro:   ");
            while(getchar() != '\n');
        } else sarrera++;
    }
    CPUk=input;
    //Core 
    sarrera=0;
    input=-666;
    printf("\nCore kopurua ezarri (1, 2, 4, 8, 16):   ");
    while (!sarrera){
        scanf("%d", &input);
        if (input != 1 && input != 2 && input != 4 && input != 8 && input != 16){
            printf("\nSarrera okerra, sartu berriro:   ");
            while(getchar() != '\n');
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
            while(getchar() != '\n');
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
    printf("First come, first served ------------ 2\n\n");
    while (!sarrera){
        scanf("%d", &input);
        if (input != 1 && input != 2 && input != 3){
            printf("\nSarrera okerra, sartu berriro:    ");
            while(getchar() != '\n');
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
                while(getchar() != '\n');
                while(getchar() != '\n');
            } else sarrera++;
        }
        quantum=input;
    } else quantum=-1;    

    printf("\n\033[1;7m---------AUKERATUTAKO KONFIGURAZIOA---------\033[0m\n");
    printf("\033[0;7m CPU kopurua: -------------------------- %d\033[0m\n",CPUk);
    printf("\033[0;7m Core kopurua: ------------------------- %d\033[0m\n",corek);
    printf("\033[0;7m Hari kopurua: ------------------------- %d\033[0m\n",threadsk);
    printf("\033[0;7m Erlojuaren maiztasuna: ---------------- %d\033[0m\n",erlojua);
    printf("\033[0;7m Aukeratutako politika: ---------------- %s\033[0m\n",politikak[politika-1]);
    if (politika==1) ("\033[0;7m Quantumaren balioa: --------------- %d\033[0m\n",quantum);
    printf("\n");

    printf("\033[1;7m--------------- HASIERAKETAK ---------------\033[0m\n");
    if (opendir("programak")){
         //Aurreko exekuzioko programak ezabatu
        printf("\033[0;7m -Aurreko programak ezabatzen --------------\n");
        system("rm -r programak");
    }
    system("mkdir programak");
    printf("\033[0;7m -Direktorio berria sortuta ----------------\n");

    disko=malloc(MEM_TAMAINA);
    if (!disko){
        printf("\033[1;31m -Errorea memoria fisikoa sortzerakoan\033[0m\n");
        exit (1);
    }
    printf("\033[0;7m -Memoria fisikoa ondo hasieratua ----------\n");

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
                CPU_list[i].core_list[j].thread_list[k].PC=0;
                CPU_list[i].core_list[j].thread_list[k].IR=malloc(100*sizeof(char));
            }
        }
    }

    printf("\033[0;7m -Hardwarea hasieratuta --------------------\n");

    waiting=0;

    printf("\033[1;92m\n------------ SIMULAZIOA MARTXAN ------------\033[0m\n");
    printf("\033[0;92m -Exekuzioa gelditzeko, sartu '.' eta 'enter'\033[0m\n");
    sleep(1);
    //Mutex
    //Haria, -, errutina, argumentuak
    pthread_create(&clock_thread, NULL, clock_routine, NULL);
    pthread_create(&pgtimer_thread, NULL, pgtimer_routine, NULL);
    pthread_create(&sctimer_thread, NULL, sctimer_routine, NULL);
    
    //Prozesu sortzailea
    //pthread_create(&processgen_thread, NULL, processgen, NULL);
    //Scheduler
    //if (politika==1)        pthread_create(&scheduler_thread, NULL, roundRobin, NULL);
    //else if (politika==2)   pthread_create(&scheduler_thread, NULL, fcfs, NULL);

    char powerInput[10];
    while (power)
    {
        scanf("%s", powerInput);
        if (strcmp(powerInput, ".") == 0)
        {
            power = 0; // Programa amaitzeko
        }
    }

    //Hariak ezabatu
    printf("\n\033[0;92m\033[7m -Hariak ezabatzen...                       \033[0m\n");
    
    pthread_detach(clock_thread);
    pthread_detach(pgtimer_thread);
    pthread_detach(sctimer_thread);

    //Memoria askatu
    printf("\033[0;92m\033[7m -Memoria askatzen...                       \033[0m\n");
    for (i=0; i<CPUk; i++){
        for (j=0; j<corek; j++){
            for (k=0; k<threadsk; k++){
                free(CPU_list[i].core_list[j].thread_list[k].IR);
            }
            free(CPU_list[i].core_list[j].thread_list);
        }
        free(CPU_list[i].core_list);
    }
    free(CPU_list);
    free(disko);

    printf("\n\033[1;92m\033[7m------------ EXEKUZIOA AMAITUTA ------------\033[0m\n\n");
}