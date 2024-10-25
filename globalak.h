#ifndef GLOBALAK
#define GLOBALAK

//errutinak
void *clock_routine(void *argv);
void *timer_routine(void *argv);

extern int done;
extern int tenp_kop;

//clock
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_cond_t cond2;

#endif 