/*
 *
 * Parallel merge sort
 * Erin McDonald erinmcdonald@sandiego.edu
 * Michael Franklin michaelfranklin@sandiego.edu
 *
 */


#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

/* Function Declarations */
void mergeSortParallel(void* rank);
extern void mergeSortSerial(int l, int r, int * arr);
void merge(int l, int lm, int m, int r, int * arr, int copy_value);
void merge2(int l, int lm, int m, int r, int * arr, int copy_value);
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
   
   //tree based reduction
    int divisor = 2;
    int difference = 1;
    long firstThread, lastThread, midThread; //first and last thread in group
    int rem;
    int thread_group;


    while (difference < threadCount) {
	    rem = threadCount % divisor;
        firstThread = (myRank - (myRank % divisor));
        lastThread = min((firstThread + divisor - 1), (threadCount -1));
        midThread = ((lastThread + firstThread) / 2) + 1;

        if(divisor > threadCount) {
            thread_group = divisor - (threadCount % divisor);
        }
        else {
            thread_group = divisor;
        }
	

        barrier();
	if(myRank < threadCount - rem) {
		mergeRec(firstIndices[firstThread], firstIndices[midThread] - 1, firstIndices[midThread], lastIndices[lastThread], \
		        thread_group, firstIndices[firstThread], firstThread, lastThread, myRank);
	}
        barrier();

        divisor *= 2;
        difference *= 2;
        for(i = myFirsti; i <= myLasti; i++) {
            vecParallel[i] = temp[i];
        }
    }

    barrier();

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

    //okay so if we check that arr is vec parallel then the serial sort on the pieces of the parallel doesn't work
    // and if we dont put a conditional around it then it's copying back during the recursive merge which isn't okay.
    // but also, that is for sure not the only prob because i checked by making another merge function which was exactly
    // the same except without the copying back and serial and partially sorted worked then i called the merge that didn't
    // copy back in the base case of the recursive function and the parallel sort still didn't work. So basically this is for
    // sure a problem but also for sure not the only problem
    int k;
    for (k = copy_value; k <= r; k++) {
        arr[k] = temp[k];
    }
    return;

} /* merge */


void merge2(int l, int lm, int m, int r, int * arr, int copy_value){
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

    return;

} /* merge 2 */


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
	    merge2(first, lmid, mid, last, vecParallel, copy_value);
    }
    else {
	    long x_mid = ((first + lmid) / 2);
	    long y_mid = binarySearch(mid, last, vecParallel[x_mid + 1]);
	    long midThread = ((lastThread + firstThread) / 2) + 1;

//x_mid is the last of X1 and y_mid is the first of Y2
            if(myRank < midThread){
    	    	mergeRec(first, x_mid, mid, y_mid - 1, (thread_group / 2), copy_value, firstThread, midThread - 1, myRank);
	    }
	    else {
	        mergeRec(x_mid + 1, lmid, y_mid, last, ((thread_group % 2) + (thread_group / 2)), (copy_value + (((x_mid - first) + 1) + (y_mid - mid))), midThread, lastThread, myRank);
	    }
    }
    return;
}/*mergeRec*/









