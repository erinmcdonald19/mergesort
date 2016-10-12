/*
 *
 * Parallel merge sort
 * Erin McDonald erinmcdonald@sandiego.edu
 * Michael Franklin michaelfranklin@sandiego.edu
 *
 */

void mergeSortParallel(void* rank);
void merge(int start1, int end1, int start2, int end2);

/* Global variables */
extern int thread_count;
extern pthread_mutex_t * lock;
extern int arraySize;
extern int SArray[], PArray[];

//TODO: create mergeSortParallel function

void mergeSortParallel(void* rank) {
    
    // Get rank of this thread.
    long myRank = (long) rank;  /* Use long in case of 64-bit system */
    
    // Compute starting and ending indices for this thread to merge.
    long quotient = sizeof(PArray) / thread_count;
    long remainder = sizeof(PArray) % thread_count;
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
    
    // TODO sort assigned subarray
    

    return NULL;
}

