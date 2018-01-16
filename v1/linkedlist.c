#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>


struct cmd getCommand() {
	struct cmd c;
	c.args = malloc(sizeof(char) * 100);
	c.name = malloc(sizeof(char) * 100);
	c.prompt = malloc(sizeof(char) * 100);
	return c;
}

void delCommand(struct cmd c) {
	free(c.args);
	free(c.name);
}

ll *getLL() {
	ll *linkedlist = malloc(sizeof(ll));
	return linkedlist;
}


struct cmd getCmd(ll* commands, char* name) {
	if (strcmp(commands->cmd->name, name)) {
		return commands->cmd;
	} else {
		return getCmd(commands, name);
	}
}
