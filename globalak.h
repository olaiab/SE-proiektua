#ifndef GLOBALAK
#define GLOBALAK
#define MAX_PROC 2000
//errutinak
void *clock_routine(void *argv);
void *timer_routine(void *argv);
void *processgen(void *argv);
void *scheduler(void *argv);

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
//clock
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_cond_t cond2;

struct pcb
{
    int pid;
    int state;      //0: funtzionatzen; 1: blokeatuta; 2: amaituta;
    int priority;   //
    int zikloak;    //zenbat ziklo bukatu arte
};

extern struct pcb proc_list[MAX_PROC];
extern struct pcb proc_waiting[MAX_PROC];

struct thread 
{
    struct pcb pcb; // Hari honek duen prozesuaren pcb
    int quantum;    // Quantuma
    int libre;      // Hariaren egoera 0=libre || 1=exekutatzen
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

#endif 