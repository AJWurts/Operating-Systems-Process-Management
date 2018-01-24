// linkedlist.h
// by Alexander Wurts
// January 2018
// For class CS3013 Project 1

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
};



struct linkedlist {
	ll *next;
	cmd cmd;

};


cmd getCommand(void);

cmd setupCommand(char* name, char* prompt, char* desc);

cmd setupUserCommand(char*);

char **parseArgString(char* args);

void delCommand(cmd c);

void printCommandWithEnum(cmd c, int count);

int size(ll *linkedlist);

ll *getLL();

cmd getFromLL(ll* commands, char* name);

cmd getIthFromLL(ll* commands, int i);

int addCmd(ll* commands, cmd command);


void printWithEnumeration(ll* commands);



#endif /* LINKED_LIST_H__ */

