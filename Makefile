CC=gcc
CFLAGS= -pthread -Wall 

mergesort: mergesort.c
	$(CC) $(CFLAGS) -o $@ $<

mergesortparallel: mergesortparallel.c
	$(CC) $(CFLAGS) -c $< -o $@


