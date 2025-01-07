#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "globalak.h"

int pg;
struct pcb proc_list[MAX_PROC];
struct pcb proc_waiting[MAX_PROC];
int waiting;

void *processgen(void *argv){
    int pos=0;
    int pid=1;
    
    while(1)
    {
        while(pg==0){}
        //Prozesu berri bat sortu
        //Prozesuak sortu
            //Listari gehitzeko: next es la posicion que tiene el ultimo proceso de la lista
        //printf("[Progn] Posizioa: %d\n",pos);
            //Hurrengo pid lortzeko: proc_list[next-1].pid es el pid del ultimo proceso
        int cpid=proc_list[pos].pid;
        int npid=cpid+1;
        pos ++;
        proc_list[pos].pid=npid;
        proc_list[pos].state=1;
        proc_list[pos].zikloak=rand()%100;   //ziklo kopurua ausaz, gehienez 10 ziklo

        printf("\n\033[1;36mProzesu berria sortuta; PID: %d\033[0m\n", npid);
        printf("\033[1;36mZikloak: %d\033[0m\n", proc_list[pos].zikloak);
        proc_waiting[waiting]=proc_list[pos];
        waiting++;
        pg=0;
    }
}