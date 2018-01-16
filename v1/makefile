
main: mc1.c linkedlist.o linkedlist.h
	cc -g -Wall -o main mc1.c linkedlist.o
	
linkedlist.o: linkedlist.c linkedlist.h
	cc -g -Wall -c linkedlist.c
	
	
clean:
	rm linkedlist.o main
	
all: main