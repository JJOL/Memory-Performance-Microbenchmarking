# Makefile for compiling the Peak Bandwidth programs
all:
	g++ -O3 naive_pbw.cpp -o pbw
	g++ mpbw.cpp -o mpbw
	gcc -O3 -fopenmp -DSTREAM_ARRAY_SIZE=100000000 stream.c -o stream

clean:
	rm -f pbw mpbw stream