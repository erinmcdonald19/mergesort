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
void merge(int l, int m, int r, int p_s);
void getIndices(long rank, long * first, long * last);
void barrier(int threads);

/* Global variables */
extern int threadCount, arraySize, count;
extern pthread_mutex_t * lock;
extern pthread_cond_t c_v;
extern int * vecSerial, * vecParallel, * temp;


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

int getSplit(long first, long last, long mid){

	printf("First: %d, Last: %d \n", first, last);

	//TODO binary search for insert position

    return -1;
}

void mergeSortParallel(void* rank) {
     
    // Get rank of this thread.
    long myRank = (long) rank;  /* Use long in case of 64-bit system */
    
    long myFirsti, myLasti;
    getIndices(myRank, &myFirsti, &myLasti);
    
    //sort assigned subarray
    mergeSortSerial(myFirsti, myLasti, 1);
    barrier(threadCount);
   //tree based reduction
    int divisor = 2;
    int difference = 1;
    long partner;
    long partnerFirst, partnerLast;

    while (difference < threadCount) {

	// Partner 1
        if (myRank % divisor == 0) {
            partner = myRank + difference;
            if (partner < threadCount) {
		getIndices(partner, &partnerFirst, &partnerLast);
		barrier(threadCount/divisor);
		printf("diff: %d, firsti: %d, partnerfirst: %d, last: %d \n",difference, myFirsti, partnerFirst, partnerLast);
		partnerLast += ((difference-1)*(arraySize/threadCount));
		printf("newLast: %d \n", partnerLast);
		merge(myFirsti, partnerFirst, partnerLast, 1);
            }
        }
	//Partner 2
        else {
	    printf("Thread %d Here \n", myRank);
            break;
        }

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


void merge(int l, int lm, int m, int r, int p_s){
    int lsaved=l;
	//int lm;
    int i;
    //if(m>0){
    //    lm = m-1;
    //}
    //else{
	//lm=0;
    //}
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

void barrier(int threads){
    pthread_mutex_lock(lock);
    count++;
    printf("count: %d \n", count);
    if(count==threads){
	count=0;
	pthread_cond_broadcast(&c_v);
    }
    else{
	while(pthread_cond_wait(&c_v, lock) != 0);
    }
    pthread_mutex_unlock(lock);
    return;
}

int binarySearch(int first, int last, int item){
    if(last<=first){
	if(item > vecParallel[first]){
	    return low+1;
	}
	else{
	    return low;
	}
    }
    int mid = (first+last)/2;
    if(item == vecParallel[mid]){
	return mid+1;
    }
    if(item> vecParallel[mid]){
	return binarySearch(mid+1, last, item);
    }
    else{
	return binarySearch(first, mid-1);
    }
}
void mergeRec(int first, int lmid, int mid, int last, int active_threads) {
    if(active_threads == 1) {
	merge(first, lmid, mid, last);
    }
    else {
	int x_mid = ((first + mid) / 2);
	int y_mid = binarySearch(???);
	if(myRank < (active_threads / 2) {
    	    mergeRec(first, x_mid, (mid + 1), y_mid);
	else {
	    mergeRec((x_mid + 1), lmid, (y_mid + 1), last);
	}
    }
	









