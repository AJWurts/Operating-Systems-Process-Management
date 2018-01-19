#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>

cmd getCommand(void) {
	cmd c;
	c.args = malloc(sizeof(char) * 128);
	c.name = malloc(sizeof(char) * 128);
	c.prompt = malloc(sizeof(char) * 100);
	c.desc = malloc(sizeof(char) * 400);
	return c;
}

cmd setupCommand(char* name, char* args, char* prompt, char* desc, int hasArgs) {
	cmd c = getCommand();
	c.name = name;
	c.args = args;
	c.prompt = prompt;
	c.desc = desc;
	c.hasArgs = hasArgs;
	return c;
}

cmd setupUserCommand(char* name, char* prompt) {
	cmd c = getCommand();

	char* args = name;
	int i = 0;
	while (args++ != NULL) {
		i++;
		if (*args == ' ') {
			args++;
			break;

		}

	}
	*(name + i) = 0;
	strcpy(c.name, name);
	c.prompt = prompt;
	c.args = args;
	c.desc = "User added command: ";
	return c;
}

void delCommand(cmd c) {
	free(c.args);
	free(c.name);
	free(c.prompt);
	free(c.desc);
}

void printCommandWithEnum(cmd c, int count) {
	printf("%d. %s\t: %s\n", count, c.name, c.desc);
}


int size(ll *linkedlist) {
	int size = 0;
	ll *current = linkedlist;
	while (current != NULL) {
		size++;
		current = current->next;
	}
	return size;
}

ll *getLL() {
	ll *linkedlist = malloc(sizeof(ll));
	return linkedlist;
}

cmd getFromLL(ll* commands, char* name) {
	if (commands != NULL) {
		int temp = strcmp(commands->cmd.name, name);
		if (temp == 0) {
			return commands->cmd;
		} else {
			return getFromLL(commands->next, name);
		}
	}
	cmd c;
	return c;
}

cmd getIthFromLLHelper(ll* commands, int i, int count) {
	if (commands != NULL) {
		if (i == count) {
			return commands->cmd;
		} else {
			return getIthFromLLHelper(commands->next, i, ++count);
		}
	}
	cmd c;
	return c;

}

cmd getIthFromLL(ll* commands, int i) {
	return getIthFromLLHelper(commands, i, 0);
}


int addCmd(ll* commands, cmd command) {
	if (commands->cmd.name == NULL) {
		commands->cmd = command;
	} else if (commands->next == NULL) {
		commands->next = getLL();
		commands->next->cmd = command;
		return 0;
	} else {
		return addCmd(commands->next, command);
	}
	return -1;
}

void printWithEnumHelper(ll* commands, int count) {
	if (commands != NULL) {
			printf("   ");
			printCommandWithEnum(commands->cmd, count++);
			printWithEnumHelper(commands->next, count);
	}
}

void printWithEnumeration(ll* commands) {
	printWithEnumHelper(commands, 0);
}




