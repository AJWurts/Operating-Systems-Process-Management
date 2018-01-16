#include <stdlib.h>
#include <stdio.h>


#ifndef LINKED_LIST
#define LINIKED_LIST

typedef struct linkedlist ll;
typedef struct command cmd;

struct command {
	char* name;
	char* args;
	char* prompt;
	int hasArgs;
};



struct linkedlist {
	ll *next;
	struct command cmd;

};




struct command getCommand();

void delCommand(struct command c);

ll *getLL();

struct commmand getCmd(ll* commands, char* name);


#endif

