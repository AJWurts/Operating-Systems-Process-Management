// linkedlist.c
// by Alexander Wurts
// January 2018
// For class CS3013 Project 1

#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>

cmd getCommand(void) {
	cmd c;
	c.name = malloc(sizeof(char) * 128);
	c.prompt = malloc(sizeof(char) * 100);
	c.args = (char**)malloc(sizeof(char*) * 2);
	c.args[1] = NULL;
	c.desc = malloc(sizeof(char) * 400);
	return c;
}

cmd setupCommand(char* name, char* prompt, char* desc) {
	cmd c = getCommand();
	c.name = name;
	c.args[0] = name;
	c.prompt = prompt;
	c.desc = desc;
	return c;
}

cmd setupUserCommand(char* input) {

	cmd c = getCommand();
	char * constructor = malloc(sizeof(char) * (40 + strlen(input)));
	strcpy(constructor, "-- Command: ");
	strcat(constructor, input);
	strcat(constructor, " --");

	char* name = strdup(input); // saves input to be saved as name for later

	// Remove program name from args list
	char* args = input;
	int i = 0;
	while (args++ != NULL) {
		i++;
		if (*args == ' ') {
			args++;
			break;
		}
	}

	//*(name + i) = 0;
	c.name = strdup(name);
	*(name + i) = 0;
	c.prompt = constructor;
	c.args = parseArgString(args); // Parse args into seperate items of array
	c.args[0] = strdup(name);
	c.desc = "User added command";
	return c;
}

char **parseArgString(char* args) {
	// Goes through a string and puts every arg separated by spaces into its own place in a new char**
	char **newArgs = (char**)malloc(sizeof(char*) * 20);
	int newArgsSize = 20;
	char cur;
	char* buffer = malloc(sizeof(char*) * 128);
	int indexInInput = 0;
	int indexInBuffer = 0;
	int indexInArgs = 1;
	while (1) {
		cur = args[indexInInput++];
		if (cur == 32 || cur == 0) {
			// if size of newArgs is too small doubles size
			if (indexInArgs > newArgsSize - 1) {
				newArgsSize *= 2;
				newArgs = realloc(newArgs, newArgsSize);
			}
			newArgs[indexInArgs] = (char*)malloc(sizeof(char) * (strlen(buffer) + 2));
			newArgs[indexInArgs++] = strdup(buffer);
			indexInBuffer = 0;
			if (cur == 0) break;
			for (int i = 0; i < 128; *(buffer++) = 0, i++);
		} else {
			buffer[indexInBuffer++] = cur;
		}
	}
	return newArgs;
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




