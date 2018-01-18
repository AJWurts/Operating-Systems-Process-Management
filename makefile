# Written by Alexander Wurts

mc0: v0/mc0.c
	cc -g -Wall -o mc0 v0/mc0.c

mc1: v1/mc1.c linkedlist.o
	cc -g -Wall -o mc1 v1/mc1.c linkedlist.o

linkedlist.o: v1/linkedlist.h v1/linkedlist.c
	cc -g -Wall -c v1/linkedlist.c v1/linkedlist.h

clean:
	rm linkedlist.o mc0 mc1
	
all: mc0 mc1