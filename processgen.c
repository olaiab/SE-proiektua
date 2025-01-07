#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "globalak.h"

int pg;
struct pcb proc_list[MAX_PROC];
struct pcb proc_waiting[MAX_PROC];
int waiting;
int pos;
int pid;
uint8_t *disko;

void orri_taula_sortu(struct pcb pcb){
    pcb.mm.pgb=(uint32_t)(sizeof(uint32_t)*1024);
    if (!pcb.mm.pgb){
        printf("\033[1;31mErrorea orri-taula sortzerakoan\033[0m\n");
        exit(1);
    }
    printf("\033[1;36mOrri-taula sortua.\033[0m\n\n");
}

void *prozesua_sortu( ){
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

    printf("\n\033[1;36mPROZESU BERRIA SORTUTA \nPID: %d \nZikloak: %d\033[0m\n", npid, proc_list[pos].zikloak);
    proc_waiting[waiting]=proc_list[pos];
    waiting++;
    orri_taula_sortu(proc_list[pos]);
}

void *programak_kargatu(struct pcb pcb, char *fitx_izena){
    FILE *fitx;
    
}

void *processgen(void *argv){  
    while(1)
    {
        while(pg==0){}
        
        prozesua_sortu();
        pg=0;
    }
}