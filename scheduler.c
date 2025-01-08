#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "globalak.h"

int sc;
struct pcb *proc_waiting[MAX_PROC];
int waiting;
int CPUk;
int corek;
int threadsk;

void lerroa_exekutatu(struct thread *thread){
    int error=1;
    if (thread->pcb.mm.code!=NULL)
    {
        if (thread->PC < (int)strlen(thread->pcb.mm.code))
        {
            error=0;
            snprintf(thread->IR, sizeof(thread->IR)+2, "%s", thread->pcb.mm.code + thread->PC);
            printf("\033[1;33m            PID %d: %.8s agindua exekutatzen --> \033[0m", thread->pcb.pid, thread->IR);
        }
    }else{
        if (thread->pcb.mm.code==NULL)printf("\033[1;31mPID: %d CODE = (null)\033[0m\n", thread->pcb.pid);
        printf("\033[1;31mErrorea lerroa irakurtzean\033[0m\n");
        return;
    }
    thread->PC+=strlen(thread->IR);
    char kodea=thread->IR[0];
    //switch case agindua exekutatzeko
    if (error==0){
        switch (kodea)
        {
        case '0':
            //LD --> CRAAAAAA
            printf("\033[1;33mLD\033[0m\n\n");
            break;
        case '1':
            //ST --> CRAAAAAA
            printf("\033[1;33mST\033[0m\n\n");
            break;
        case '2':
            //ADD --> CRRR----
            printf("\033[1;33mADD\033[0m\n\n");
            break;
        case 'F':
            //EXIT
            printf("\033[1;33mEXIT\033[0m\n\n");
            thread->pcb.exit=1;
            break;
        default:
            break;
        }
    }
    
}   

void *roundRobin(){
    int i; //CPU kop iteratzeko
    int j; //Core kop iteratzeko
    int k; //Hari kop iteratzeko
    int h; //Blokeo zerrenda iteratzeko
    
    if (waiting>0){
        printf("\n\033[1m-----ITXAROTE ZERRENDA-----\033[0m\n");
                for (h=0; h<waiting; h++){                                        //Zerrenda inprimatzen
                        printf("    \033[1m-PID: %d\033[0m\n", proc_waiting[h]->pid);
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
                    proc_waiting[waiting]=&CPU_list[i].core_list[j].thread_list[k].pcb;  //Blokeo listara sartu
                    waiting++;
                    CPU_list[i].core_list[j].thread_list[k].libre=0;                    //Haria askatu
                    CPU_list[i].core_list[j].thread_list[k].quantum=quantum;            //Quantuma berrezarri
                    CPU_list[i].core_list[j].thread_list[k].pcb.pid=-1;
                    CPU_list[i].core_list[j].thread_list[k].PC=0;
                    CPU_list[i].core_list[j].thread_list[k].IR=malloc(100*sizeof(char));
                }
                if (CPU_list[i].core_list[j].thread_list[k].libre!=0)                   //Haria ez dago libre :(
                {
                    printf("\033[1;35m            PID: %d prozesua martxan.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                    printf("            Amaitzeko zikloak: %d\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                    printf("            Quantum-a: %d\n", CPU_list[i].core_list[j].thread_list[k].quantum);
                    CPU_list[i].core_list[j].thread_list[k].quantum --;                 //Quantuma murriztu
                    CPU_list[i].core_list[j].thread_list[k].pcb.zikloak --;             //Amaitzeko zikloak murriztu
                    lerroa_exekutatu(&CPU_list[i].core_list[j].thread_list[k]);
                    //esto no va por ciclos, este if habria que hacerlo con una variable
                    if (CPU_list[i].core_list[j].thread_list[k].pcb.exit == 1)
                    {
                        printf("\033[1;92m            %d. prozesua amaitu da.\033[0m\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                        CPU_list[i].core_list[j].thread_list[k].pcb.state=2;            //Prozesua amaitu da
                        CPU_list[i].core_list[j].thread_list[k].libre=0;                //Haria askatu
                        CPU_list[i].core_list[j].thread_list[k].quantum=quantum;        //Quantuma berrezarri
                        CPU_list[i].core_list[j].thread_list[k].PC=0;
                        CPU_list[i].core_list[j].thread_list[k].IR=malloc(100*sizeof(char));
                    }
                }
                else
                {   
                if (waiting>0)
                {                                                                       //Haria libre dago :)
                    CPU_list[i].core_list[j].thread_list[k].pcb=*proc_waiting[0];       //Blokeo zerrendako lehena esleitu
                    CPU_list[i].core_list[j].thread_list[k].pcb.state=0;
                    for (h=0; h<waiting-1; h++){                                        //Zerrenda eguneratzen
                        proc_waiting[h]=proc_waiting[h+1];
                    }
                    waiting--;
                    printf("\033[1;34m            PID: %d prozesua esleitu da.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                    printf("            Amaitzeko zikloak: %d\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                    printf("            Quantum-a: %d\n", CPU_list[i].core_list[j].thread_list[k].quantum);
                    CPU_list[i].core_list[j].thread_list[k].libre=1;
                    CPU_list[i].core_list[j].thread_list[k].quantum--;
                    lerroa_exekutatu(&CPU_list[i].core_list[j].thread_list[k]);
                    CPU_list[i].core_list[j].thread_list[k].pcb.zikloak--;
                }
                else printf("           Ez daude blokeatutako prozesurik\n\n");
                }
            }
        }
    }
}

void *fcfs(){
    int i;
    int j;
    int k;
    int h;
    
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
                        CPU_list[i].core_list[j].thread_list[k].pcb=*proc_waiting[0];        //Blokeo zerrendako lehena esleitu
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
}

void *scheduler(){
    if (politika==1) roundRobin();
    else if (politika==2) fcfs();
}