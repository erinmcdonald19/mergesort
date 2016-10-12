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

}