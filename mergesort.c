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
#include <semaphore.h>

#include <unistd.h> // For sysconf

/* Function declarations */
void usage(char* prog_name);

/* Global constants */
const int MAX_THREADS = 64;
const int MAX_NAME_LEN = 10;

/* Global variables */
int thread_count;
pthread_mutex_t * lock;
int arraySize;


/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    
    // Get number of threads from command line
    if (argc != 3)
        usage(argv[0]);
    thread_count = (int) strtol(argv[1], NULL, 10);
    if (thread_count <= 0 || thread_count > MAX_THREADS)
        usage(argv[0]);
    arraySize = (int) strtol(argv[2], NULL, 10);
    if(arraySize =

    // For timing
    struct timeval  tv1, tv2;
    
    // Fill array with random numbers
    srandom((unsigned int) time(NULL));
    for (long i = 0; i < VECTOR_LEN; i++) {
        vec[i] = rand();
    }
    
    // Compute sum with serial code
    gettimeofday(&tv1, NULL); // start timing
    sumSerial = arraySumSerial(VECTOR_LEN, vec);
    gettimeofday(&tv2, NULL); // stop timing
    double serialTime = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
    (double) (tv2.tv_sec - tv1.tv_sec);
    
    // Compute sum with parallel threads
    long thread;  // Use long in case of a 64-bit system
    gettimeofday(&tv1, NULL);
    
    // Intialize lock, and sumParallel to 0.
    if ((lock = malloc(sizeof(pthread_mutex_t))) == NULL) {
        printf("Error allocating memory for lock\n");
        exit(1);
    }
    int code;
    if ((code = pthread_mutex_init(lock, NULL)) != 0) {
        printf("Error initializing lock\n");
        exit(code);
    }
    sumParallel = 0;
    
    // Create and launch the threads.
    pthread_t* thread_handles;
    thread_handles = malloc (thread_count*sizeof(pthread_t));
    for (thread = 1; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL,
                       arraySumParallel, (void*) thread);
    }
    arraySumParallel(0);
    
    // Wait for all threads to finish.
    for (thread = 1; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
    free(thread_handles);
    
    // Stop timing.
    gettimeofday(&tv2, NULL);
    double parallelTime = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
    (double) (tv2.tv_sec - tv1.tv_sec);
    
    // Check that parallel sum is correct.
    if (sumSerial != sumParallel) {
        printf("Incorrect parallel sum.");
    }
    
    // Print results.
    printf("Serial time = %e\n", serialTime);
    printf("Parallel time = %e\n", parallelTime);
    double speedup = serialTime / parallelTime;
    double efficiency = speedup / thread_count;
    printf("Speedup = %e\n", speedup);
    printf("Efficiency = %e\n", efficiency);
    
    
    return 0;
}  /* main */

/*------------------------------------------------------------------
 * Compute sum of array ar and return. */
long arraySumSerial(long len, int ar []) {
    long sum = 0;
    for (long i = 0; i < len; i++) {
        sum += ar[i];
    }
    return sum;
}

/*-------------------------------------------------------------------*/
/* Compute sum of global array vec in parallel. */
void * arraySumParallel(void* rank) {
    
    // Get rank of this thread.
    long myRank = (long) rank;  /* Use long in case of 64-bit system */
    
    // Compute starting and ending indices for this thread to sum.
    long quotient = VECTOR_LEN / thread_count;
    long remainder = VECTOR_LEN % thread_count;
    long myCount;
    long myFirsti, myLasti;
    if (myRank < remainder) {
        myCount = quotient + 1;
        myFirsti = myRank * myCount;
    }
    else {
        myCount = quotient;
        myFirsti = myRank * myCount + remainder;
    }
    myLasti = myFirsti + myCount;
    
    // Compute partial sum for this thread, and store in global partial sum array.
    long temp = 0;
    for (long i = myFirsti; i < myLasti; i++) {
        temp += vec[i];
    }
    
    // Add partial sum to sumParallel in critical section.
    pthread_mutex_lock(lock);
    sumParallel += temp;
    pthread_mutex_unlock(lock);
        
    return NULL;
}

/*-------------------------------------------------------------------*/
void usage(char* prog_name) {
    fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
    fprintf(stderr, "0 < number of threads <= %d\n", MAX_THREADS);
    exit(0);
}  /* Usage */
