/*
 *
 * Parallel merge sort
 * Erin McDonald erinmcdonald@sandiego.edu
 * Michael Franklin michaelfranklin@sandiego.edu
 *
 */


#include <pthread.h>

/* Function Declarations */
void mergeSortParallel(void* rank);
extern void mergeSortSerial(int l, int r, int parallel_subsort);
void merge(int l, int m, int r, int p_s);
void getIndices(long rank, long * first, long * last);

/* Global variables */
extern int thread_count, arraySize;
extern pthread_mutex_t * lock;
extern int * SArray, * PArray, * tmp;

void getIndices(long myRank, long *myFirsti, long *myLasti){
    // Compute starting and ending indices for this thread to merge.
    long quotient = arraySize / thread_count;
    long remainder = arraySize % thread_count;
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
    printf("I'm thread: %d with First: %d, Last %d \n", myRank, *myFirsti, *myLasti);

    return;
}

void mergeSortParallel(void* rank) {
 
    // Get rank of this thread.
    long myRank = (long) rank;  /* Use long in case of 64-bit system */
    long myFirsti, myLasti;
    getIndices(myRank, &myFirsti, &myLasti);
    
    //sort assigned subarray
    mergeSortSerial(myFirsti, myLasti, 1);

   //TODO sync here with condition variable

   //tree based reduction
    int divisor = 2;
    int difference = 1;
    long partner;
    
    while (difference < thread_count) {
	// Partner 1
        if (myRank % divisor == 0) {
            partner = myRank + difference;
            if (partner < thread_count) {
		long partnerFirst, partnerLast;
		getIndices(partner, &partnerFirst, &partnerLast);
		int mid = (myLasti-myFirsti)/2;
		int split, k;
		for(k=partnerFirst; k=partnerLast; k++){
		    if(PArray[mid]<PArray[k]){
			continue;
		    }
		    else{
			split = k;
		    }
		}
                //TODO condition variable
		//TODO merge x1 (myFirst -> split -1) y1 (myPartnerFirst -> split -1)
            }
        }
	//Partner 2
        else {
            //TODO condition variable
	    //TODO merge x2 y2
            break;
        }
        divisor *= 2;
        difference *= 2;
    }

    


    if(rank ==0){
	int i;
    	printf("Partially sorted: \n");
    	for(i=0; i<arraySize; i++){
		printf("%d \n", PArray[i]);
   	 }
    }
    
    
    return;
}/* mergeSortParallel */


void merge(int l, int m, int r, int p_s){
    int lsaved=l;
    int lm, i;
    if(m>0){
        lm = m-1;
    }
    else{
	lm=0;
    }
    i = l;

    int * arr;
    if(p_s==1){
	arr = PArray;
    }
    else{
	arr = SArray;
    }

    while(l <= lm && m <= r){
	if(arr[l] <= arr[m]){
	    tmp[i] = arr[l];
	    i++;
	    l++;
	}
	else{
	    tmp[i]=arr[m];
	    i++;
	    m++;
	}
    }
    while(l <= lm){
	tmp[i] = arr[l];
	l++;
	i++;
    }
    while(m <= r){
	tmp[i] = arr[m];
	m++;
	i++;
    }
    int k;
    for(k=lsaved; k<= r; k++){
	arr[k] = tmp[k];
    }
    return;

} /* merge */

