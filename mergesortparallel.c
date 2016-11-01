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
extern void mergeSortSerial(int l, int r, int parallel_subsort);
void merge(int l, int lm, int m, int r, int p_s, int copy_value);
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

    //testing barrier
    //printf("Here 1 \n");
    barrier();
    //printf("Here 2 \n");


    long myFirsti, myLasti;
    int i;
    getIndices(myRank, &myFirsti, &myLasti);
    firstIndices[myRank] = myFirsti;
    lastIndices[myRank] = myLasti;
    
    //sort assigned subarray
    mergeSortSerial(myFirsti, myLasti, 1);
    barrier();
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


void merge(int l, int lm, int m, int r, int p_s, int copy_value){
    int lsaved=l;
    int i;
    //running serially
    if(p_s == 0) {
        if(m>0) {
            lm = m - 1;
        }
        else{
            lm = 0;
        }
    }

    i = copy_value; // takes in to account for "right" thread merges that have variable starting copy indices due to
                    // the variability of the length of the "left" string's merge after the binary search. For anything
                    // other than the "right" thread, copy_value = l, though the right thread could have a copy_value
                    // equal to l

    int * arr;
    if(p_s==1){
	    arr = vecParallel;
    }
    else{
	    arr = vecSerial;
    }

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
    int k;
    if(p_s == 0) {
        for (k = lsaved; k <= r; k++) {
            arr[k] = temp[k];
        }
	    memcpy(vecSerial, arr, arraySize);
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
    if (last <= first) {
        if (item > vecParallel[first]) {
            return first + 1;
        } else {
            return first;
        }
    }
    int mid = (first + last) / 2;
    if (item == vecParallel[mid]) {
        return mid + 1;
    }
    if (item > vecParallel[mid]) {
        return binarySearch(mid + 1, last, item);
    } else {
        return binarySearch(first, mid - 1, item);
    }
}/*binarySearch*/


void mergeRec(long first, long lmid, long mid, long last, int thread_group, long copy_value, long firstThread, long lastThread, long myRank) {


    if(thread_group == 1) {
	    merge(first, lmid, mid, last, 1, copy_value);
    }
    else {
	    long x_mid = ((first + lmid) / 2);
	    long y_mid = binarySearch(mid, last, vecParallel[x_mid]);
	    if(y_mid > lastIndices[threadCount - 1]) {
		y_mid--;
	    }
	    long midThread = ((lastThread + firstThread) / 2) + 1;


//printing out values for debugging
        if(myRank < midThread){
            printf("\nThread Group = %d\nFirst thread is %lu and last thread is %lu and middle thread is %lu\nI'm going to the left and my rank is %lu and my bounds are %lu, %lu, and %lu, %lu, y_mid is %lu\n\n", thread_group, firstThread, lastThread, midThread, myRank, first, x_mid - 1, mid, y_mid - 1, y_mid);
        }
        else {
            printf("\nThread Group = %d\nFirst thread is %lu and last thread is %lu and middle thread is %lu\nI'm going to the right and my rank is %lu and my bounds are %lu, %lu, and %lu, %lu, y_mid is %lu\n\n", thread_group, firstThread, lastThread, midThread, myRank, x_mid, lmid, y_mid, last, y_mid);
        }

	printf("right side copy value is: %lu\n", (((x_mid - first) + (y_mid - mid)) + 1));

        if(myRank < midThread){
    	    mergeRec(first, x_mid - 1, mid, y_mid - 1, (thread_group / 2), first, firstThread, midThread - 1, myRank);
	    }
	    else {
	        mergeRec(x_mid, lmid, y_mid, last, (thread_group / 2), (((x_mid - first) + (y_mid - mid)) + 1), midThread, lastThread, myRank);
	    }
    }
    return;
}/*mergeRec*/









