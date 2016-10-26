/*
 * Globals for merge sort
 *
 * Erin McDonald & Michael Franklin
 */
#include <pthread.h>

/* Global variables */
int thread_count, arraySize;
pthread_mutex_t * lock;
pthread_cond_t * merged;
int * SArray, * PArray,  * tmp;
