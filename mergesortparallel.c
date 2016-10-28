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
void merge(int l, int lm, int m, int r, int p_s);
void getIndices(long rank, long * first, long * last);
void barrier();
void mergeRec(int first, int lmid, int mid, int last, int thread_group, int copy_value, int midThread);

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
    
    long myFirsti, myLasti;
    getIndices(myRank, &myFirsti, &myLasti);
    firstIndices[myRank] = myFirsti;
    lastIndices[myRank] = myLasti;
    
    //sort assigned subarray
    mergeSortSerial(myFirsti, myLasti, 1);
    barrier(threadCount);
   //tree based reduction
    int divisor = 2;
    int difference = 1;
    long firstThread, lastThread; //first and last thread in group

    while (difference < threadCount) {

        firstThread = (myRank - (myRank % divisor));
        lastThread = (firstThread + divisor - 1);
        midThread = (lastThread + firstThread) / 2;

        barrier();
	    mergeRec(firstIndices[firstThread], lastIndices[firstThread], firstIndices[midThread], lastIndices[lastThread], divisor, firstIndices[midThread], midThread);
        barrier();
        divisor *= 2;
        difference *= 2;
    }

    
    if(rank ==0){
	int i;
    	printf("Partially sorted: \n");
    	for(i=0; i<arraySize; i++){
		printf("%d \n", vecParallel[i]);
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
    i = l;

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
    for(k=lsaved; k<= r; k++){
	arr[k] = temp[k];
    }
    if(p_s==1){
	memcpy(vecParallel, arr, arraySize);
    }
    else{
	memcpy(vecSerial, arr, arraySize);
    }
    return;

} /* merge */

void barrier(){
    pthread_mutex_lock(lock);
    count++;
    printf("count: %d \n", count);
    if(count==threadCount){
	count=0;
	pthread_cond_broadcast(&c_v);
    }
    else{
	while(pthread_cond_wait(&c_v, lock) != 0);
    }
    pthread_mutex_unlock(lock);
    return;
}

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
        return binarySearch(first, mid - 1);
    }
}
void mergeRec(int first, int lmid, int mid, int last, int thread_group, int copy_value) {
    if(thread_group == 1) {
	    merge(first, lmid - 1, mid, last, copy_value);
    }
    else {
	    int x_mid = ((first + lmid) / 2);
	    int y_mid = binarySearch(mid, last, vecParallel[x_mid]);
        if(){
    	    mergeRec(first, x_mid, (mid + 1), y_mid, (thread_group / 2), first, );
	    }
	    else {
	        mergeRec((x_mid + 1), lmid, (y_mid + 1), last, (thread_group / 2), (x_mid + y_mid));
	    }
    }
}









