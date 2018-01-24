// linkiedlist2.c
// by Alexander Wurts
// January 2018
// For class CS3013 Project 1

#include "linkedlist2.h"

#include <stdlib.h>
#include <string.h>


// Returns an allocated command
cmd getCommand(void) {
	cmd c;
	c.name = malloc(sizeof(char) * 128);
	c.prompt = malloc(sizeof(char) * 100);
	c.args = (char**)malloc(sizeof(char*) * 2);
	c.args[1] = NULL;
	c.desc = malloc(sizeof(char) * 400);
	return c;
}

// Setup a command given name, prompt and desc.
cmd setupCommand(char* name, char* prompt, char* desc) {
	cmd c = getCommand();
	c.name = name;
	c.args[0] = name;
	c.prompt = prompt;
	c.desc = desc;
	c.isBckg = 0;
	return c;
}

// Sets up a user command given an input string
cmd setupUserCommand(char *input) {
	cmd c = getCommand();

	// Constructs Prompt for when process runs
	char * constructor = malloc(sizeof(char) * (40 + strlen(input)));
	strcpy(constructor, "-- Command: ");
	strcat(constructor, input);
	strcat(constructor, " --");

	char* name = strdup(input); // saves input to be saved as name for later


	// Handles if process is a background process
	int isBckg = 0;
	if (input[strlen(input) - 1] == '&'){ // Checks to see if background Command
		isBckg = 1;
		int i = strlen(input);
		// While Loop to remove & and any whitespace after command
		while ((int)input[i] == 38
				||
				!(33 <= (int)input[i] && (int)input[i] <= 126)) { //  while last character is not valid character
			input[i] = '\0'; // set character to 0
			i--;
		}
	}

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
	c.isBckg = isBckg;
	return c;
}

// Parses the argument input string
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

// Counts members of the linked list and returns int size
int size(ll *linkedlist) {
	int size = 0;
	ll *current = linkedlist;
	while (current != NULL) {
		size++;
		current = current->next;
	}
	return size;
}

// Returns an allocated linked list
ll *getLL() {
	ll *linkedlist = malloc(sizeof(ll));
	return linkedlist;
}

// Gets command with the given name from the  LInked List
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

// Get ith elemenet helper
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

// Gets the ith element from a linked list
cmd getIthFromLL(ll* commands, int i) {
	return getIthFromLLHelper(commands, i, 0);
}


// Adds a command to the end of the linked list
ll* addCmd(ll* commands, cmd command) {
	if (commands == NULL) { // If linked list is empty
		commands = getLL();
		commands->cmd = command;
		commands->next = NULL;
		return commands;
	}
	if (commands->cmd.name == NULL) { // If command is empty
		commands->cmd = command;
	} else if (commands->next == NULL) { // If next item is empty
		commands->next = getLL();
		commands->next->cmd = command;
	} else { // None of the above so check next node
		addCmd(commands->next, command);
	}
	return commands; // happens quite often heh
}


// Removes a command with the same name from the list
ll* delCmd(ll* commands, cmd command) {

	if (commands == NULL) {
		// should never happen but blocks program from crashing
		return commands;
	}

	if (commands->next != NULL) { // If next isn't null and name equals cmd.name deletes it
		if (strcmp(commands->next->cmd.name, command.name)) {
			commands->next = commands->next->next;

			return commands;
		}
		delCmd(commands->next, command);
	} else  {


		commands = NULL;
		return commands;
	}
	return NULL;

}

// Print linked list with number helper
void printWithEnumHelper(ll* commands, int count) {
	if (commands != NULL) {
			printf("   ");
			printCommandWithEnum(commands->cmd, count++);
			printWithEnumHelper(commands->next, count);
	}
}

// Print Linked List with numbers
void printWithEnumeration(ll* commands) {
	printWithEnumHelper(commands, 0);
}

// Free a command hopefully without crashing
void freeCommand(cmd command) {
	free(command.name);
	free(command.prompt);
	free(command.desc);
}

// Free LinkedList
void freeLL(ll* linkedlist) {
	if (linkedlist != NULL) {
		if (linkedlist->next != NULL) {
			freeLL(linkedlist->next);
		} else {
			freeCommand(linkedlist->cmd);
			free(linkedlist);
		}
	}
}
