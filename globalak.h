
#ifndef GLOBALAK
#define GLOBALAK
#define MAX_PROC 2000
#define ORRI_TAMAINA 4096
#define MEM_TAMAINA 1<<24   //mirar bien esto
#include <stdint.h>
//errutinak
void *clock_routine(void *argv);
void *timer_routine(void *argv);
void *processgen(void *argv);
void *prozesua_sortu();
void *scheduler();

extern int done;
extern int tenp_kop;
extern int pg;
extern int sc;
extern int erlojua;
extern int CPUk;
extern int corek;
extern int threadsk;
extern int quantum;
extern int pgtick;
extern int sctick;
extern int waiting;
extern int politika;
extern int create_now;
extern int power;
extern long codestart, datastart;
//clock
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_cond_t cond2;

struct mm
{
    uint32_t pgb;
    char *code;
    char *data;
};

struct pcb
{
    int pid;        //prozesuaren pid
    int state;      //0: funtzionatzen; 1: blokeatuta; 2: amaituta;
    int zikloak;    //zenbat ziklo bukatu arte
    struct mm mm;   //memory management
    int exit;       //0: amaitu gabe; 1: exit agindua irakurrita
    int PC;         //exekuzioaren PC
};

extern struct pcb proc_list[MAX_PROC];
extern struct pcb *proc_waiting[MAX_PROC];

struct TLB
{
    int tamaina;
    void *TLB_list;
};

struct thread 
{
    struct pcb pcb; // Hari honek duen prozesuaren pcb
    int quantum;    // Quantuma
    int libre;      // Hariaren egoera 0=libre || 1=exekutatzen
    void *PTBR;
    int PC;
    char *IR;
    void *MMU;
    struct TLB TLB;
    int erregistroak[16];
};

struct core // CPU bakoitzean dauden core-ak
{
    struct thread *thread_list;
};

struct CPU // CPU-ak
{
    struct core *core_list;
};
extern struct CPU *CPU_list;

extern uint8_t *disko;

void *buennoc[sizeof(int)];
int buennkop;
#endif 