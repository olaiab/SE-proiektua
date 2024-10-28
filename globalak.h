#ifndef GLOBALAK
#define GLOBALAK

//errutinak
void *clock_routine(void *argv);
void *timer_routine(void *argv);
void *processgen(void *argv);
void *scheduler(void *argv);

extern int done;
extern int tenp_kop;
extern struct pcb proc_list[];

//clock
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_cond_t cond2;

#endif 