/*
 * Globals for merge sort
 *
 * Erin McDonald & Michael Franklin
 */
#include <pthread.h>

/* Global variables */
int thread_count, arraySize;
pthread_mutex_t * lock;
int * SArray, * PArray,  * tmp;
