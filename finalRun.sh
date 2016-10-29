#!/bin/sh
for j in 1 2 4 8 16 32 64 
do
    for i in 1000 10000 100000 1000000 10000000 100000000
    do
	~/Desktop/mergesort/mergesort $j $i
    done
done
