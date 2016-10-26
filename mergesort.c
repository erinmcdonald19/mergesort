/*
 *
 * Merge Sort
 * Erin McDonald erinmcdonald@sandiego.edu
 * Michael Franklin michaelfranklin@sandiego.edu
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>
#include "mergesortparallel.h"
#include "mergesortparallel.c"

#include <unistd.h> // For sysconf

/* Function declarations */
void usage(char* prog_name);
void mergeSortSerial(int l, int r, int parallel_subsort);

/* Global constants */
const int MAX_THREADS = 64;

/* Global variables */
int thread_count, arraySize;
pthread_mutex_t * lock;
pthread_cond_t * merged;
int * SArray, * PArray, * tmp;

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    
    // Get number of threads from command line
    if (argc != 3)
        usage(argv[0]);
    thread_count = (int) strtol(argv[1], NULL, 10);
    if (thread_count <= 0 || thread_count > MAX_THREADS)
        usage(argv[0]);
    arraySize = (int) strtol(argv[2], NULL, 10);
    if(arraySize <= 0)
        usage(argv[0]);

    // allocate space for arrays
    SArray = malloc(sizeof(int)*arraySize);
    PArray = malloc(sizeof(int)*arraySize);
    tmp = malloc(sizeof(int)*arraySize);
    int val,i;
    // Fill array with random numbers
    printf("Unsorted: \n");
    srand(time(NULL));
    for(i = 0; i < arraySize; i++){
        val = rand() %10;
        SArray[i] = val;
	PArray[i] = val;
	printf("%d \n", val);
    }
    

    // For timing
   struct timeval  tv1, tv2;
  
    // Compute sum with serial code
    gettimeofday(&tv1, NULL); // start timing
    mergeSortSerial(0, arraySize-1,0);
    printf("Serially sorted: \n");
    for(i=0; i<arraySize; i++){
	printf("%d \n", SArray[i]);
    }
    gettimeofday(&tv2, NULL); // stop timing
    double serialTime = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
    (double) (tv2.tv_sec - tv1.tv_sec);
 
    // Compute sum with parallel threads
    long thread;  // Use long in case of a 64-bit system
    gettimeofday(&tv1, NULL);
    
    // Intialize lock
    if ((lock = malloc(sizeof(pthread_mutex_t))) == NULL) {
        printf("Error allocating memory for lock\n");
        exit(1);
    }
    int code;
    if ((code = pthread_mutex_init(lock, NULL)) != 0) {
        printf("Error initializing lock\n");
        exit(code);
    }

    // Create the semaphores.
    merged = malloc(sizeof(pthread_cond_t *) * thread_count);
    
    // Intialize the semaphores
    for (thread = 0; thread < thread_count; thread++) {
        pthread_cond_t * cond_var;
	cond_var = malloc(sizeof(pthread_cond_t));
        pthread_cond_init(cond_var, NULL);
	merged[thread] = * cond_var;
    }

    // Create and launch the threads.
    pthread_t* thread_handles;
    thread_handles = malloc (thread_count*sizeof(pthread_t));
    for (thread = 1; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL,
                       mergeSortParallel, (void*) thread);
    }
    mergeSortParallel(0); 
    
    // Wait for all threads to finish.
    for (thread = 1; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
    free(thread_handles);
    
    // Stop timing.
    gettimeofday(&tv2, NULL);
    double parallelTime = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
    (double) (tv2.tv_sec - tv1.tv_sec);
    
    //TODO check that sorted lists match (that parallel sorted correctly)
  
    // Print results.
    printf("Serial time = %e\n", serialTime);
    printf("Parallel time = %e\n", parallelTime);
    double speedup = serialTime / parallelTime;
    double efficiency = speedup / thread_count;
    printf("Speedup = %e\n", speedup);
    printf("Efficiency = %e\n", efficiency);
	
    return 0;
}  /* main */

void mergeSortSerial(int l, int r, int parallel_subsort){
    int m;
    if(r>l){
	m=(r+l)/2 +1;
	mergeSortSerial(l, m-1, parallel_subsort);
	mergeSortSerial(m,r, parallel_subsort);
	merge(l,m,r, parallel_subsort);
    }
    return;
} /* mergeSortSerial */ 



void usage(char* prog_name) {
    fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
    fprintf(stderr, "0 < number of threads <= %d\n", MAX_THREADS);
    exit(0);
}  /* Usage */
