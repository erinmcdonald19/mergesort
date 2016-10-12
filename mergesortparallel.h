/*
 * Globals for merge sort
 *
 * Erin McDonald & Michael Franklin
 */
#include <pthread.h>

/* Global variables */
int thread_count;
pthread_mutex_t * lock;
int arraySize;
int SArray[], PArray[];
