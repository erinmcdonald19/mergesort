CC=gcc
CFLAGS= -pthread -Wall -o

mergesort: mergesort.c
	$(CC) $(CFLAGS) $@ $<
