#include <pthread.h>

int done;
int tenp_kop;


//clock
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_cond_t cond2;

//PCB
struct pcb
{
    int pid;
};

struct pcb proc_list[];
