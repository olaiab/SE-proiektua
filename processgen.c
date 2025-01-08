#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "globalak.h"

int pg;
struct pcb proc_list[MAX_PROC];
struct pcb *proc_waiting[MAX_PROC];
int waiting;
int pos;
int pid;
uint8_t *disko;

void orri_taula_sortu(struct pcb *pcb){
    pcb->mm.pgb=(uint32_t)(sizeof(uint32_t)*1024);
    if (!pcb->mm.pgb){
        printf("\033[1;31mErrorea orri-taula sortzerakoan\033[0m\n");
        exit(1);
    }
    printf("\033[1;36mOrri-taula sortua.\033[0m\n\n");
}

void *programak_kargatu(struct pcb *pcb, char *fitx_izena){
    FILE *fitx=fopen(fitx_izena, "r");
    if (fitx==NULL){
        printf("\033[1;31mErrorea fitxategia kargatzerakoan\033[0m\n");
        exit(1);
    }

    int lerro_zenb=0;
    int size_code=0;
    int size_data=0;
    char lerroa[24];
    char codehelb[24];
    char datahelb[24];
    long datastart, codestart;
    pcb->mm.code=NULL;
    pcb->mm.data=NULL;

    while (fgets(lerroa, sizeof(lerroa), fitx)!=NULL){
        if(sscanf(lerroa, ".text %s", codehelb)==1){
            codestart=strtol(codehelb, NULL, 16);
        }
        else if(sscanf(lerroa, ".data %s", datahelb)==1){
            datastart=strtol(datahelb, NULL, 16);
        }
        else{

            if(lerro_zenb<((datastart)+8)/4){
                size_code+=strlen(lerroa)+1;
                if(pcb->mm.code==NULL){
                    pcb->mm.code=(char *)malloc(size_code);
                }else pcb->mm.code=(char *)realloc(pcb->mm.code,size_code);
                strcat(pcb->mm.code, lerroa);
            }
            else //(lerro_zenb<(atoi(datahelb))+2)
            {
                size_data+=strlen(lerroa)+1;
                if(pcb->mm.data==NULL){
                    pcb->mm.data=(char *)malloc(size_data);
                }else pcb->mm.data=(char *)realloc(pcb->mm.data,size_data);
                strcat(pcb->mm.data, lerroa);
            }
        }
        lerro_zenb++;
    }
    /*
    printf("PID: %d\n", pcb->pid);
    printf("CODE\n%s\n\n", pcb->mm.code);
    printf("DATA\n%s\n\n", pcb->mm.data);
    */
    fclose(fitx);    
}

void *prozesua_sortu(){
    //Prozesu berri bat sortu
        //Prozesuak sortu
            //Listari gehitzeko: next es la posicion que tiene el ultimo proceso de la lista
        //printf("[Progn] Posizioa: %d\n",pos);
            //Hurrengo pid lortzeko: proc_list[next-1].pid es el pid del ultimo proceso
    char fitx_izena[32];
    int cpid=proc_list[pos].pid;
    int npid=cpid+1;
    pos ++;
    proc_list[pos].pid=npid;
    proc_list[pos].state=1;
    proc_list[pos].exit=0;
    proc_list[pos].zikloak=rand()%100;   //ziklo kopurua ausaz, gehienez 10 ziklo
    printf("\n\033[1;36mPROZESU BERRIA SORTUTA \nPID: %d \nZikloak: %d\033[0m\n", npid, proc_list[pos].zikloak);
    
    sprintf(fitx_izena, "prometheus/prog%03d.elf", proc_list[pos].pid);
    programak_kargatu(&proc_list[pos], fitx_izena);
    orri_taula_sortu(&proc_list[pos]);
    proc_waiting[waiting]=&proc_list[pos];
    waiting++;
}
/*
void *processgen(void *argv){  
    while(1)
    {
        while(pg==0){}
        prozesua_sortu();
        pg=0;
    }
}
*/