/*
 *
 * Parallel merge sort
 * Erin McDonald erinmcdonald@sandiego.edu
 * Michael Franklin michaelfranklin@sandiego.edu
 *
 */


#include <pthread.h>
#include <string.h>

/* Function Declarations */
void mergeSortParallel(void* rank);
extern void mergeSortSerial(int l, int r, int * arr);
void merge(int l, int lm, int m, int r, int * arr, int copy_value);
void getIndices(long rank, long * first, long * last);
void barrier();
void mergeRec(long first, long lmid, long mid, long last, int thread_group, long copy_value, long firstThread, long lastThread, long myRank);

/* Global variables */
extern int threadCount, arraySize, count;
extern pthread_mutex_t * lock;
extern pthread_cond_t c_v;
extern int * vecSerial, * vecParallel, * temp;
extern long * firstIndices, * lastIndices;

void getIndices(long myRank, long *myFirsti, long *myLasti){
    // Compute starting and ending indices for this thread to merge.
    long quotient = arraySize / threadCount;
    long remainder = arraySize % threadCount;
    long myCount;
    if (myRank < remainder) {
        myCount = quotient + 1;
        *myFirsti = myRank * myCount;
    }
    else {
        myCount = quotient;
        *myFirsti = myRank * myCount + remainder;
    }
    *myLasti = *myFirsti + myCount - 1;

    return;
}

void mergeSortParallel(void* rank) {
     
    // Get rank of this thread.
    long myRank = (long) rank;  /* Use long in case of 64-bit system */

    //testing
    //printf("Here 1 \n");
    barrier();
    //printf("Here 2 \n");


    long myFirsti, myLasti;
    int i;
    getIndices(myRank, &myFirsti, &myLasti);
    firstIndices[myRank] = myFirsti;
    lastIndices[myRank] = myLasti;
    
    //sort assigned subarray
    mergeSortSerial(myFirsti, myLasti, vecParallel);
    barrier();
    for(i = myFirsti; i <= myLasti; i++) {
        vecParallel[i] = temp[i];
    }
    barrier();
    if(rank ==0) {
        int j;
        printf("\"Partially sorted\": \n");
        for (j = 0; j < arraySize; j++) {
            printf("%d \n", vecParallel[j]);
        }
    }
   //tree based reduction
    int divisor = 2;
    int difference = 1;
    long firstThread, lastThread, midThread; //first and last thread in group


    while (difference < threadCount) {

        firstThread = (myRank - (myRank % divisor));
        lastThread = (firstThread + divisor - 1);
        midThread = ((lastThread + firstThread) / 2) + 1;

        barrier();
	    mergeRec(firstIndices[firstThread], lastIndices[firstThread], firstIndices[midThread], lastIndices[lastThread], \
                divisor, firstIndices[midThread], firstThread, lastThread, myRank);
        barrier();

        divisor *= 2;
        difference *= 2;
        for(i = myFirsti; i <= myLasti; i++) {
            vecParallel[i] = temp[i];
        }
    }

    if(rank ==0){
	    int j;
    	printf("\"Parallel Sorted\": \n");
    	for(j=0; j<arraySize; j++){
		printf("%d \n", vecParallel[j]);
   	    }
    }
    
    
    return;
}/* mergeSortParallel */


void merge(int l, int lm, int m, int r, int * arr, int copy_value){
    int lsaved=l;
    int i;
    i = copy_value;

    while(l <= lm && m <= r){
	    if(arr[l] <= arr[m]){
	        temp[i] = arr[l];
	        i++;
	        l++;
	    }
	    else{
	        temp[i]=arr[m];
	        i++;
	        m++;
	    }
    }
    while(l <= lm){
	    temp[i] = arr[l];
	    l++;
	    i++;
    }
    while(m <= r){
	    temp[i] = arr[m];
	    m++;
	    i++;
    }

    //okay so if we check that arr is vec parallel then the serial sort on the pieces of the parallel doesn't work and if we dont put a conditional around it then it's copying back during the recursive merge which isn't okay. but also, that is for sure not the only prob because i checked by making another merge function which was exactly the same except without the copying back and serial and partially sorted worked then i called the merge that didn't copy back in the base case of the recursive function and the parallel sort still didn't work. So basically this is for sure a problem but also for sure not the only problem
    int k;
    for (k = lsaved; k <= r; k++) {
        arr[k] = temp[k];
    }
    return;

} /* merge */


void barrier(){
    pthread_mutex_lock(lock);
    count++;
    if(count==threadCount){
	count=0;
	pthread_cond_broadcast(&c_v);
    }
    else{
	while(pthread_cond_wait(&c_v, lock) != 0);
    }
    pthread_mutex_unlock(lock);
    return;
}/*Barrier*/

int binarySearch(int first, int last, int item) {
    if (last < first) {
        return first;
    }
    int mid = (first + last) / 2;
    if (item > vecParallel[mid]) {
        return binarySearch(mid + 1, last, item);
    } else {
        return binarySearch(first, mid - 1, item);
    }
}/*binarySearch*/


void mergeRec(long first, long lmid, long mid, long last, int thread_group, long copy_value, long firstThread, long lastThread, long myRank) {


    if(thread_group == 1) {
	    merge(first, lmid, mid, last, vecParallel, copy_value);
    }
    else {
        printf("mid is %d, last is %d\n", mid, last);

	    long x_mid = ((first + lmid) / 2);
	    long y_mid = binarySearch(mid, last, vecParallel[x_mid + 1]);
	    long midThread = ((lastThread + firstThread) / 2) + 1;

//lol
//printing out values for debugging
        if(myRank < midThread){
            printf("\nThread Group = %d\nFirst thread is %lu and last thread is %lu and middle thread is %lu\nI'm going to the left and my rank is %lu and my bounds are %lu, %lu, and %lu, %lu, y_mid is %lu\n\n", thread_group, firstThread, lastThread, midThread, myRank, first, x_mid - 1, mid, y_mid - 1, y_mid);
        }
        else {
            printf("\nThread Group = %d\nFirst thread is %lu and last thread is %lu and middle thread is %lu\nI'm going to the right and my rank is %lu and my bounds are %lu, %lu, and %lu, %lu, y_mid is %lu\n\n", thread_group, firstThread, lastThread, midThread, myRank, x_mid, lmid, y_mid, last, y_mid);
        }

	printf("right side copy value is: %lu\n", (((x_mid - first) + (y_mid - mid)) + 1));
//x_mid is the last of X1 and y_mid is the first of Y2
        if(myRank < midThread){
    	    mergeRec(first, x_mid, mid, y_mid - 1, (thread_group / 2), copy_value, firstThread, midThread - 1, myRank);
	    }
	    else {
	        mergeRec(x_mid + 1, lmid, y_mid, last, (thread_group / 2), (copy_value + (((x_mid - first) + 1) + (y_mid - mid))), midThread, lastThread, myRank);
	    }
    }
    return;
}/*mergeRec*/









