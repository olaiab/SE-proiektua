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
pthread_mutex_t lock;

int hex_to_dec(int R){
    if (R>='0' && R<='9'){
        R=R-'0';
    }else if (toupper(R)>='A' && toupper(R)<='F'){
        R=toupper(R)-'A'+10;
    }
    return R;
}

void lerroa_exekutatu(struct thread *thread){
    pthread_mutex_lock(&lock);
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
        printf("\033[1;31m            Errorea lerroa irakurtzean\033[0m\n");
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
            printf("\033[1;33mLD R%d, 0x%s\033[0m\n", R1, helbidea);
            break;
        case '1':
            //ST --> CRAAAAAA
            R1=hex_to_dec(thread->IR[1]); 
            strncpy(helbidea, &thread->IR[2], 6);
            helb=strtoul(helbidea, NULL, 16);
            printf("\033[1;33mST R%d, 0x%s\033[0m\n", R1, helbidea);
            break;
        case '2':
            //ADD --> CRRR----
            R1=hex_to_dec(thread->IR[1]); 
            R2=hex_to_dec(thread->IR[2]); 
            R3=hex_to_dec(thread->IR[3]); 
            printf("\033[1;33mADD R%d, R%d, R%d\033[0m\n", R1, R2, R3);
            break;
        case 'F':
            //EXIT
            printf("\033[1;33mEXIT\033[0m\n");
            thread->pcb.exit=1;
            break;
        default:
            break;
        }
    }
    pthread_mutex_unlock(&lock);
}   

int haria_askatu(struct thread *thread){
    thread->libre=0;
    thread->quantum=quantum;
    thread->PC=0;
    memset(thread->IR, 0, 100);
    thread->pcb.exit=0;
}

void garbitu(struct pcb *wait[MAX_PROC]){
    for (int i=0; i<waiting-1; i++){
        if (wait[i]->state==0){
            for (int j=i; j<waiting-1;j++){
                wait[j]=wait[j+1];
            }
            waiting--;
        }
    }
}

void prozesua_esleitu(struct thread *thread){
    thread->pcb=*proc_waiting[0];                                           //Blokeo zerrendako lehena esleitu
    thread->pcb.state=0;
    thread->PC=thread->pcb.PC;
    if (waiting>1){
        for (int h=0; h<waiting-1; h++){                                        //Zerrenda eguneratzen
            proc_waiting[h]=proc_waiting[h+1];
        } 
    }
    waiting--;
    printf("\033[1;34m            PID: %d prozesua esleitu da.\033[0m\n", thread->pcb.pid);
    if (politika==1) printf("\033[1;34m            Quantum-a: %d\033[0m\n", thread->quantum);
    thread->libre=1;
    thread->quantum--;
}

void prozesua_blokeatu(struct thread *thread){
    thread->pcb.state=1;                //Prozesua blokeatuta
    thread->pcb.PC=thread->PC;
    proc_waiting[waiting]=&thread->pcb; //Blokeo listara sartu
    waiting++;
}

void *roundRobin(){
    int i; //CPU kop iteratzeko
    int j; //Core kop iteratzeko
    int k; //Hari kop iteratzeko
    int h; //Blokeo zerrenda iteratzeko

    if (waiting>0){
        garbitu(proc_waiting);
        printf("\n\033[1m-----ITXAROTE ZERRENDA-----\033[0m\n");
        for (h=0; h<waiting; h++){                                        //Zerrenda inprimatzen
                printf("    \033[1m-PID: %d EGOERA: %d\033[0m\n", proc_waiting[h]->pid, proc_waiting[h]->state);
        }
    }else printf("\n\033[1mEz dago prozesurik itxarote zerrendan\033[0m\n");
    for (i=0; i<CPUk; i++)
    {
        printf("\n\033[1m%d. CPU\033[0m\n", i);
        for (j=0; j<corek; j++)
        {
            printf("\n\033[1m    %d. CORE\033[0m\n", j);
            for (k=0; k<threadsk; k++)
            {
                printf("\n\033[1m        %d. HARIA\033[0m\n", k);
                if (CPU_list[i].core_list[j].thread_list[k].libre!=0)                   //Haria ez dago libre :(
                {
                    printf("\033[1;35m            PID: %d prozesua martxan.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                    printf("\033[1;35m            Quantum-a: %d\033[0m\n", CPU_list[i].core_list[j].thread_list[k].quantum);
                    lerroa_exekutatu(&CPU_list[i].core_list[j].thread_list[k]);
                    CPU_list[i].core_list[j].thread_list[k].quantum --;                   //Quantuma murriztu
                    if (CPU_list[i].core_list[j].thread_list[k].quantum==0)                 //Quantuma amaitu da
                    {
                        if (CPU_list[i].core_list[j].thread_list[k].pcb.exit == 1)
                        {
                            printf("\033[1;92m            %d. prozesua amaitu da.\033[0m\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                            CPU_list[i].core_list[j].thread_list[k].pcb.state=2;            //Prozesua amaitu da
                            haria_askatu(&CPU_list[i].core_list[j].thread_list[k]);
                        }else{
                            printf("\033[1;31m            PID: %d prozesua blokeatu da.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                            garbitu(proc_waiting);
                            prozesua_blokeatu(&CPU_list[i].core_list[j].thread_list[k]);
                            haria_askatu(&CPU_list[i].core_list[j].thread_list[k]);
                        }
                        
                    }
                    if (CPU_list[i].core_list[j].thread_list[k].pcb.exit == 1)          
                    {
                        printf("\033[1;92m            %d. prozesua amaitu da.\033[0m\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                        CPU_list[i].core_list[j].thread_list[k].pcb.state=2;            //Prozesua amaitu da
                        haria_askatu(&CPU_list[i].core_list[j].thread_list[k]);
                    }
                }
                else                                                                    //Haria libre dago :)
                {   
                if (waiting>0)
                {
                    garbitu(proc_waiting);
                    prozesua_esleitu(&CPU_list[i].core_list[j].thread_list[k]);
                    lerroa_exekutatu(&CPU_list[i].core_list[j].thread_list[k]);
                    if (quantum==1){
                        if (CPU_list[i].core_list[j].thread_list[k].quantum==0)                 //Quantuma amaitu da
                        {
                            printf("\033[1;31m            PID: %d prozesua blokeatu da.\033[0m\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                            garbitu(proc_waiting);
                            prozesua_blokeatu(&CPU_list[i].core_list[j].thread_list[k]);
                            haria_askatu(&CPU_list[i].core_list[j].thread_list[k]);
                        }
                    }
                }
                else printf("           Ez daude blokeatutako prozesurik.\n\n");
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
                        printf("    \033[1m-PID: %d EGOERA: %d\033[0m\n", proc_waiting[h]->pid, proc_waiting[h]->state);
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
                    lerroa_exekutatu(&CPU_list[i].core_list[j].thread_list[k]);
                    if (CPU_list[i].core_list[j].thread_list[k].pcb.exit == 1)
                    {
                        printf("\033[1;92m            %d. prozesua amaitu da.\033[0m\n\n", CPU_list[i].core_list[j].thread_list[k].pcb.pid);
                        CPU_list[i].core_list[j].thread_list[k].pcb.state=2;            //Prozesua amaitu da
                        haria_askatu(&CPU_list[i].core_list[j].thread_list[k]);
                    }
                }
                else if (CPU_list[i].core_list[j].thread_list[k].libre==0)              //Haria libre dago :)
                {
                if (waiting>0)
                {              
                    prozesua_esleitu(&CPU_list[i].core_list[j].thread_list[k]);
                    lerroa_exekutatu(&CPU_list[i].core_list[j].thread_list[k]);
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