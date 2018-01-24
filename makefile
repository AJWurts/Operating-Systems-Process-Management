# Written by Alexander Wurts
ARGS = -g -Wall

mc0: v0/mc0.c
	cc $(ARGS) -o mc0 v0/mc0.c

mc1: v1/mc1.c linkedlist.o
	cc $(ARGS) -o mc1 v1/mc1.c linkedlist.o
	
mc2: v2/mc2.c linkedlist2.o
	cc $(ARGS) -o mc2 v2/mc2.c linkedlist2.o 
	
linkedlist2.o: v2/linkedlist2.h v2/linkedlist2.c
	cc $(ARGS) -c v2/linkedlist2.c v2/linkedlist2.h

linkedlist.o: v1/linkedlist.h v1/linkedlist.c
	cc $(ARGS) -c v1/linkedlist.c
	

clean:
	rm -f *.o mc0 mc1 mc2 linkedlist.o linkedlist2.o v2/linkedlist2.h.gch v1/linkedlist.h.gch
	
all: mc0 mc1 mc2