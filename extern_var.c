#include <pthread.h>
#include "globalak.h"


int done;
int tenp_kop;
int pg;
int sc;


//clock
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_cond_t cond2;


struct pcb proc_list[MAX_PROC];
