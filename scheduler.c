#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include "globalak.h"

int sc;
struct pcb *proc_waiting[MAX_PROC];
int waiting;
int CPUk;
int corek;
int threadsk;
pthread_mutex_t waiting_mutex;

int hex_to_dec(int R){
    if (R>='0' && R<='9'){
        R=R-'0';
    }else if (toupper(R)>='A' && toupper(R)<='F'){
        R=toupper(R)-'A'+10;
    }
    return R;
}
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
    int R1, R2, R3;
    char helbidea[6];
    int helb;
    //switch case agindua exekutatzeko
    if (error==0){
        switch (kodea)
        {
        case '0':
            //LD --> CRAAAAAA
            R1=hex_to_dec(thread->IR[1]);  
            strncpy(helbidea, &thread->IR[2], 6);
            helb=strtoul(helbidea, NULL, 16);
            //thread->erregistroak[R1]=;
            printf("\033[1;33mLD R%d, 0x%s\033[0m\n\n", R1, helbidea);
            break;
        case '1':
            //ST --> CRAAAAAA
            R1=hex_to_dec(thread->IR[1]); 
            strncpy(helbidea, &thread->IR[2], 6);
            helb=strtoul(helbidea, NULL, 16);
            printf("\033[1;33mST R%d, 0x%s\033[0m\n\n", R1, helbidea);
            break;
        case '2':
            //ADD --> CRRR----
            R1=hex_to_dec(thread->IR[1]); 
            R2=hex_to_dec(thread->IR[2]); 
            R3=hex_to_dec(thread->IR[3]); 
            printf("\033[1;33mADD R%d, R%d, R%d\033[0m\n\n", R1, R2, R3);
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
                    printf("\033[1;31m            PID: %d prozesua blokeatu da.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                    //printf("            Geratzen diren zikloak: %d\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                    CPU_list[i].core_list[j].thread_list[k].pcb.state=1;                //Prozesua blokeatuta
                    pthread_mutex_lock(&waiting_mutex);
                    proc_waiting[waiting]=&CPU_list[i].core_list[j].thread_list[k].pcb; //Blokeo listara sartu
                    waiting++;
                    pthread_mutex_unlock(&waiting_mutex);
                    CPU_list[i].core_list[j].thread_list[k].libre=0;                    //Haria askatu
                    CPU_list[i].core_list[j].thread_list[k].quantum=quantum;            //Quantuma berrezarri
                    //CPU_list[i].core_list[j].thread_list[k].pcb.pid=-1;               
                    CPU_list[i].core_list[j].thread_list[k].PC=0;                       //PCa berrezarri
                    CPU_list[i].core_list[j].thread_list[k].IR=malloc(100*sizeof(char));//IRa berrezarri
                }
                if (CPU_list[i].core_list[j].thread_list[k].libre!=0)                   //Haria ez dago libre :(
                {
                    printf("\033[1;35m            PID: %d prozesua martxan.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                    //printf("            Amaitzeko zikloak: %d\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                    printf("\033[1;35m            Quantum-a: %d\033[0m\n", CPU_list[i].core_list[j].thread_list[k].quantum);
                    CPU_list[i].core_list[j].thread_list[k].quantum --;                   //Quantuma murriztu
                    //CPU_list[i].core_list[j].thread_list[k].pcb.zikloak --;             //Amaitzeko zikloak murriztu
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
                    pthread_mutex_lock(&waiting_mutex);
                    for (h=0; h<waiting-1; h++){                                        //Zerrenda eguneratzen
                        proc_waiting[h]=proc_waiting[h+1];
                    }
                    waiting--;
                    pthread_mutex_unlock(&waiting_mutex);
                    printf("\033[1;34m            PID: %d prozesua esleitu da.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                    //printf("            Amaitzeko zikloak: %d\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                    printf("\033[1;34m            Quantum-a: %d\033[0m\n", CPU_list[i].core_list[j].thread_list[k].quantum);
                    CPU_list[i].core_list[j].thread_list[k].libre=1;
                    CPU_list[i].core_list[j].thread_list[k].quantum--;
                    lerroa_exekutatu(&CPU_list[i].core_list[j].thread_list[k]);
                    //CPU_list[i].core_list[j].thread_list[k].pcb.zikloak--;
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
                if (CPU_list[i].core_list[j].thread_list[k].libre!=0)                   //Haria ez dago libre :(
                {
                    printf("\033[1;35m            PID: %d prozesua martxan.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                    //printf("            Amaitzeko zikloak: %d\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                    //CPU_list[i].core_list[j].thread_list[k].pcb.zikloak --;             //Amaitzeko zikloak murriztu
                    lerroa_exekutatu(&CPU_list[i].core_list[j].thread_list[k]);
                    //esto no va por ciclos, este if habria que hacerlo con una variable
                    if (CPU_list[i].core_list[j].thread_list[k].pcb.exit == 1)
                    {
                        printf("\033[1;92m            %d. prozesua amaitu da.\033[0m\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                        CPU_list[i].core_list[j].thread_list[k].pcb.state=2;            //Prozesua amaitu da
                        CPU_list[i].core_list[j].thread_list[k].libre=0;                //Haria askatu
                        CPU_list[i].core_list[j].thread_list[k].PC=0;
                        CPU_list[i].core_list[j].thread_list[k].IR=malloc(100*sizeof(char));
                    }
                }
                else if (CPU_list[i].core_list[j].thread_list[k].libre==0)
                {
                    if (waiting>0)
                {                                                                       //Haria libre dago :)
                    CPU_list[i].core_list[j].thread_list[k].pcb=*proc_waiting[0];       //Blokeo zerrendako lehena esleitu
                    CPU_list[i].core_list[j].thread_list[k].pcb.state=0;
                    pthread_mutex_lock(&waiting_mutex);
                    for (h=0; h<waiting-1; h++){                                        //Zerrenda eguneratzen
                        proc_waiting[h]=proc_waiting[h+1];
                    }
                    waiting--;
                    pthread_mutex_unlock(&waiting_mutex);
                    printf("\033[1;34m            PID: %d prozesua esleitu da.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                    //printf("            Amaitzeko zikloak: %d\n", CPU_list[i].core_list[j].thread_list[k].pcb.zikloak);
                    CPU_list[i].core_list[j].thread_list[k].libre=1;
                    lerroa_exekutatu(&CPU_list[i].core_list[j].thread_list[k]);
                    //CPU_list[i].core_list[j].thread_list[k].pcb.zikloak--;
                }
                else printf("           Ez daude blokeatutako prozesurik\n\n");
                }
            }
        }
    }
}

void *scheduler(){
    if (politika==1) roundRobin();
    else if (politika==2) fcfs();
}