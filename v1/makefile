
main: mc1.c linkedlist.o
	cc -g -Wall -o main mc1.c
	
linkedlist.o: linkedlist.c linkedlist.h
	cc -g -Wall -c linkedlist.c linkedlist.h
	
	
clean:
	rm linkedlist.o main
	
all: main