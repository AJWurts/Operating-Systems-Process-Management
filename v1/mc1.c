#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"


int main(int argc, char* argv) {
	ll *commands = getLL();
	loadInitialCommands();
	int pid;
	if ((pid = fork()) != 0) {
		wait(pid);
		printf("Parent");
	} else {
		struct command cmd = getCommand(commands, "ls");
		char *args[2];
		args[0] = strdup(cmd->name);
		args[1] = NULL;
		printf(cmd->prompt);
		execvp(args[0], args);
	}



	while (1) {
		// hard coded - a, c, e, pwd
		// part of commmands 0, 1, 2, ...


	
	}	
	return 0;
}


int loadInitialCommands(ll *commands) {
	struct command whoami = getCommand();
	whoami->name = "whoami";
	whoami->hasArgs = 0;
	whoami->args = NULL;
	commands->cmd = whoami;

	ll *next = getLL();
	commands->next = next;

	struct command last = getCommand();
	last->name = "last";
	last->hasArgs = 0;
	last->args = NULL;
	commands->next->cmd = last;

	ll nextnext = getLL();
	next->next = nextnext;

	struct command ls = getCommand();
	ls->name = "ls";
	ls->hasArgs = 1;
	ls->args = "";
	ls->prompt = "-- Directory Listing --";
	nextnext->cmd = ls;

}
