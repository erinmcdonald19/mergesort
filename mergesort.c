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
//#include <mergesortparallel.h>

#include <unistd.h> // For sysconf

/* Function declarations */
void usage(char* prog_name);
void mergeSortSerial(int tmp[], int l, int r);
void merge(int tmp[], int l, int m, int r);

/* Global constants */
const int MAX_THREADS = 64;
const int MAX_NAME_LEN = 10;

int SArray[8];

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    /*
    // Get number of threads from command line
    if (argc != 3)
        usage(argv[0]);
    thread_count = (int) strtol(argv[1], NULL, 10);
    if (thread_count <= 0 || thread_count > MAX_THREADS)
        usage(argv[0]);
    arraySize = (int) strtol(argv[2], NULL, 10);
    if(arraySize <= 0)
        usage(argv[0]);

    // For timing
    struct timeval  tv1, tv2;

    int val;
    // Fill array with random numbers
    srandom((unsigned int) time(NULL));
    for (long i = 0; i < arraySize; i++) {
        val = rand();
        SArray[i] = val;
        PArray[i] = val;
    }
    
    // Compute sum with serial code
    gettimeofday(&tv1, NULL); // start timing
    SArray = mergeSortSerial(VECTOR_LEN, vec); //TODO: change params
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
    
    // Create and launch the threads.
    pthread_t* thread_handles;
    thread_handles = malloc (thread_count*sizeof(pthread_t));
    for (thread = 1; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL,
                       mergeSortParallel, (void*) thread);
    }
    mergeSortParallel(0); //TODO change params
    
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
    */

    int* SArray;
    SArray = malloc(sizeof(int)*8);
    int k;
    for(k=0;k<8;k++){
	SArray[k]=rand() %7;
    }
    int j;
    printf("Serial: \n");
    for(j=0; j< sizeof(SArray); j++){
	printf("%d \n", SArray[j]);
    }
    int temporary[sizeof(SArray)];
    mergeSortSerial(temporary, 0, sizeof(SArray)-1);
    int i;
    printf("Sorted: \n");
    for(i=0; i< sizeof(SArray); i++){
	printf("%d \n", SArray[i]);
    }
	
    free(SArray);    

    return 0;
}  /* main */

void mergeSortSerial(int tmp[], int l, int r){
    int m;
    if(r>l){
	m=(r+l)/2 +1;
	mergeSortSerial(tmp, l, m-1);
	mergeSortSerial(tmp, m,r);
	merge(tmp,l,m,r);
    }
    return;
} /* mergeSortSerial */ 

void merge(int tmp[], int l, int m, int r){
    int lm, len, i;
    lm = m-1;
    len = r - l + 1;
    i = l;

    while(l <= lm && m <= r){
	if(SArray[l] <= SArray[m]){
	    tmp[i] = SArray[l];
	    i++;
	    l++;
	}
	else{
	    tmp[i]=SArray[m];
	    i++;
	    m++;
	}
    }
    while(l <= lm){
	tmp[i] = SArray[l];
	l++;
	i++;
    }
    while(m <= r){
	tmp[i] = SArray[m];
	m++;
	i++;
    }
    int k;
    for(k=0; k<= len; k++){
	SArray[k] = tmp[k];
    }
    return;

} /* merge */

void usage(char* prog_name) {
    fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
    fprintf(stderr, "0 < number of threads <= %d\n", MAX_THREADS);
    exit(0);
}  /* Usage */
