#include <pthread.h>

int done;
int tenp_kop;

//clock
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_cond_t cond2;
