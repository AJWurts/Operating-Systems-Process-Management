#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef LINKED_LIST_H__
#define LINKED_LIST_H__

typedef struct linkedlist ll;
typedef struct command cmd;

struct command {
	char* name;
	char** args;
	char* prompt;
	char* desc;
	int pid;
	int isBckg;
	struct timeval *startTime;
};



struct linkedlist {
	ll *next;
	cmd cmd;

};


cmd getCommand(void);

cmd setupCommand(char* name, char* prompt, char* desc);

cmd setupUserCommand(char* name, char* prompt, int isBckg);

char **parseArgString(char* args);

ll* addCmd(ll* commands, cmd command);

ll* delCmd(ll* commands, cmd command);

void printCommandWithEnum(cmd c, int count);

int size(ll *linkedlist);

ll *getLL();

cmd getFromLL(ll* commands, char* name);

cmd getIthFromLL(ll* commands, int i);

void printWithEnumeration(ll* commands);

void freeCommand(cmd command);

void freeLL(ll* linkedlist);


#endif /* LINKED_LIST_H__ */

