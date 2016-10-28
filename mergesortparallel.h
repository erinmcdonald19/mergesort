/*
 * Globals for merge sort
 *
 * Erin McDonald & Michael Franklin
 */
#include <pthread.h>

/* Global variables */
int threadCount, arraySize, count;
pthread_mutex_t * lock;
pthread_cond_t c_v;
int *  vecSerial, * vecParallel,  * temp;
long * firstIndices, * lastIndices;
