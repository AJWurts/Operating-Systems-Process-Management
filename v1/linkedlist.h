#include <stdlib.h>
#include <stdio.h>

#ifndef LINKED_LIST_H__
#define LINKED_LIST_H__

typedef struct linkedlist ll;
typedef struct command cmd;

struct command {
	char* name;
	char* args;
	char* prompt;
	char* desc;
	int hasArgs;
};



struct linkedlist {
	ll *next;
	cmd cmd;

};


cmd getCommand(void);

cmd setupCommand(char* name, char* args, char* prompt, char* desc, int hasArgs);

cmd setupUserCommand(char* name, char*prompt);

void delCommand(cmd c);

void printCommandWithEnum(cmd c, int count);

int size(ll *linkedlist);

ll *getLL();

cmd getFromLL(ll* commands, char* name);

cmd getIthFromLL(ll* commands, int i);

int addCmd(ll* commands, cmd command);


void printWithEnumeration(ll* commands);



#endif /* LINKED_LIST_H__ */

