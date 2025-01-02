#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "globalak.h"

int pg;
struct pcb proc_list[MAX_PROC];

void *processgen(void *argv){
    int pos=0;
    int pid=1;
    while(1){
        while(pg==0){}
        //Prozesu berri bat sortu
        //Prozesuak sortu
            //Listari gehitzeko: next es la posicion que tiene el ultimo proceso de la lista
        printf("[Progn] Posizioa: %d\n",pos);
            //Hurrengo pid lortzeko: proc_list[next-1].pid es el pid del ultimo proceso
        int cpid=proc_list[pos].pid;
        int npid=cpid+1;
        pos ++;
        printf("[Progn] Prozesu berria; PID: %d\n", npid);
        proc_list[pos].pid=npid;
        pg=0;
    }
}