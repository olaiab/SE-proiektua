#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "globalak.h"

int sc;
struct pcb proc_waiting[MAX_PROC];
int waiting;
int CPUk;
int corek;
int threadsk;

void *roundRobin(void *argv){
    int i; //CPU kop iteratzeko
    int j; //Core kop iteratzeko
    int k; //Hari kop iteratzeko
    int h; //Blokeo zerrenda iteratzeko
    while (1)
    {
        while(sc==0){}
        if (waiting>0){
            printf("\n\033[1m-----ITXAROTE ZERRENDA-----\033[0m\n");
                    for (h=0; h<waiting; h++){                                        //Zerrenda inprimatzen
                            printf("    \033[1m-PID: %d\033[0m\n", proc_waiting[h].pid);
                        }
        }else printf("\n\033[1mEz dago prozesurik itxarote zerrendan\033[0m\n");
        for (i=0; i<CPUk; i++)
        {
            printf("\n\033[1m%d. CPU\033[0m\n", i);
            for (j=0; j<corek; j++)
            {
                printf("\033[1m    %d. CORE\033[0m\n", j);
                for (k=0; k<threadsk; k++)
                {
                    printf("\033[1m        %d. HARIA\033[0m\n", k);
                    if (CPU_list[i].core_list[j].thread_list[k].quantum==0)                 //Quantuma amaitu da
                    {
                        printf("\033[1;31m            %d. prozesua blokeatu da.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                        printf("            Geratzen diren zikloak: %d\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                        CPU_list[i].core_list[j].thread_list[k].pcb.state=1;                //Prozesua blokeatuta
                        proc_waiting[waiting]=CPU_list[i].core_list[j].thread_list[k].pcb;  //Blokeo listara sartu
                        waiting++;
                        CPU_list[i].core_list[j].thread_list[k].libre=0;                    //Haria askatu
                        CPU_list[i].core_list[j].thread_list[k].quantum=quantum;            //Quantuma berrezarri
                        CPU_list[i].core_list[j].thread_list[k].pcb.pid=-1;
                    }
                    if (CPU_list[i].core_list[j].thread_list[k].libre!=0)                   //Haria ez dago libre :(
                    {
                        printf("\033[1;35m            PID: %d prozesua martxan.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                        printf("            Amaitzeko zikloak: %d\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                        printf("            Quantum-a: %d\n\n", CPU_list[i].core_list[j].thread_list[k].quantum);
                        CPU_list[i].core_list[j].thread_list[k].quantum --;                 //Quantuma murriztu
                        CPU_list[i].core_list[j].thread_list[k].pcb.zikloak --;             //Amaitzeko zikloak murriztu
                        if (CPU_list[i].core_list[j].thread_list[k].pcb.zikloak <= 0)
                        {
                            printf("\033[1;92m            %d. prozesua amaitu da.\033[0m\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                            CPU_list[i].core_list[j].thread_list[k].pcb.state=2;            //Prozesua amaitu da
                            CPU_list[i].core_list[j].thread_list[k].libre=0;                //Haria askatu
                            CPU_list[i].core_list[j].thread_list[k].quantum=quantum;        //Quantuma berrezarri
                        }                        
                    }
                    else
                    {   
                    if (waiting>0)
                    {                                                                       //Haria libre dago :)
                        CPU_list[i].core_list[j].thread_list[k].pcb=proc_waiting[0];        //Blokeo zerrendako lehena esleitu
                        CPU_list[i].core_list[j].thread_list[k].pcb.state=0;
                        for (h=0; h<waiting-1; h++){                                        //Zerrenda eguneratzen
                            proc_waiting[h]=proc_waiting[h+1];
                        }
                        waiting--;
                        printf("\033[1;34m            PID: %d prozesua esleitu da.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                        printf("            Amaitzeko zikloak: %d\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                        printf("            Quantum-a: %d\n\n", CPU_list[i].core_list[j].thread_list[k].quantum);
                        CPU_list[i].core_list[j].thread_list[k].libre=1;
                        CPU_list[i].core_list[j].thread_list[k].quantum--;
                        CPU_list[i].core_list[j].thread_list[k].pcb.zikloak--;
                    }
                    else printf("           Ez daude blokeatutako prozesurik\n\n");
                    }
                }
            }
        }
        sc=0;
    }
}

void *fcfs(void *argv){
    int i;
    int j;
    int k;
    int h;
    while (1)
    {
        while(sc==0){}
        for (i=0; i<CPUk; i++)
        {
            printf("\n\033[1m%d. CPU\033[0m\n", i);
            for (j=0; j<corek; j++)
            {
                printf("\033[1m    %d. CORE\033[0m\n", j);
                for (k=0; k<threadsk; k++)
                {
                    printf("\033[1m        %d. HARIA\033[0m\n", k);
                    if (CPU_list[i].core_list[j].thread_list[k].libre!=0)                   //Haria ez dago libre :(
                    {
                        printf("\033[1;35m            PID: %d prozesua martxan.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                        printf("            Amaitzeko zikloak: %d\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                        CPU_list[i].core_list[j].thread_list[k].pcb.zikloak --;             //Amaitzeko zikloak murriztu
                        if (CPU_list[i].core_list[j].thread_list[k].pcb.zikloak <= 0)
                        {
                            printf("\033[1;92m            %d. prozesua amaitu da\033[0m\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                            printf("\033[1;34m            PID: %d prozesua esleitu da.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                            printf("            Amaitzeko zikloak: %d\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                            CPU_list[i].core_list[j].thread_list[k].pcb.state=2;            //Prozesua amaitu da
                            CPU_list[i].core_list[j].thread_list[k].libre=0;                //Haria askatu
                        } 
                    }
                    else
                    {
                        if (waiting>0)
                        {
                            CPU_list[i].core_list[j].thread_list[k].pcb=proc_waiting[0];        //Blokeo zerrendako lehena esleitu
                            CPU_list[i].core_list[j].thread_list[k].pcb.state=0;
                            for (h=0; h<waiting-1; h++){                                        //Zerrenda eguneratzen
                                proc_waiting[h]=proc_waiting[h+1];
                            }
                            waiting--;
                            CPU_list[i].core_list[j].thread_list[k].libre=1;
                            CPU_list[i].core_list[j].thread_list[k].pcb.zikloak--;
                        }
                        else printf("           Ez daude prozesurik itxaroten\n\n");
                    }
                }
            }
        }
        sc=0;    
    }
}