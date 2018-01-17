#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "linkedlist.h"

int loadInitialCommands(ll*);
void printCharacterCommands();
long runCommand(cmd);

int main(int argc, char* argv[]) {
	ll *commands = getLL();

	loadInitialCommands(commands);
	char* input = malloc(sizeof(char) * 130);
	char* directory = malloc(sizeof(char) * 128);
	directory = argv[0];
	argv[0][strlen(argv[0]-5)] = 0;

	while (1) {
		// hard coded - a, c, e, pwd
		// part of commmands 0, 1, 2, ...

		// Print current commands:
		printf("===== Mid-Day Commander, v1 =====\n");
		printf("G'Day, Commander! What command would you like to run?\n");
		printWithEnumeration(commands);
		printCharacterCommands();
		printf("Option?: ");


		scanf("%s", input);
		getchar();
		printf("\n");

		if (48 <= input[0] && input[0] <= 57) {
			int i = atoi(input);
			cmd command = getIthFromLL(commands, i);
			runCommand(command);
		} else {
			switch (input[0]) {
				case 'a': {
					printf("-- Add a Command --\n");
					printf("Command to add?: ");
					scanf("%[^\n]%*c", input);
					cmd temp = setupUserCommand(input, "User added command");
					addCmd(commands, temp);
					printf("Okay, added with ID %d\n\n", size(commands) - 1);
					break;
				}
				case 'c':
					printf("-- Change Directory --\n");
					printf("Enter new directory: ");
					scanf("%s", directory);
					chdir(input);
					printf("\n\n");
					break;
				case 'p': {
					printf("-- Current Directory --\nDirectory: ");
					printf("%s\n\n", directory);
					break;
				}
				case 'e': {
					exit(0);
					break;
				}

				default: {
					printf("Invalid command please try again\n\n");
				}

			}
		}
	}
	return 0;
}

long runCommand(cmd command) {
	struct timeval *startTime, *endTime;
	startTime = malloc(sizeof(struct timeval));
	endTime = malloc(sizeof(struct timeval));
	long totalTime;
	char* args[2];
	args[0] = malloc(sizeof(char) * 128);
	args[1] = malloc(sizeof(char) * 128);

	if (!strcmp(command.name, "ls")) {
		printf("\n-- Directory Listing --\n");
		printf("Arguments?: ");
		fgets(args[0], sizeof args[0], stdin);
		if (*args[0] == '\n')
			*args[0] = ' ';
		else
			*(args[0] + strlen(args[0]) - 1) = 0;
		printf("Path?: ");
		fgets(args[1], sizeof args[1], stdin);
		if (*args[1] == '\n')
				*args[1] = ' ';
		else *(args[1] + strlen(args[1]) - 1) = 0;
		//"%[^\n]%*c"
		printf("\n\n");
		// Concat inputed args tp create command args.
		command.args = args[1];
		strcat(command.args, " ");
		strcat(command.args, args[0]);
	}

	gettimeofday(startTime, NULL);
	int ret = fork();
	if (ret < 0) {
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (ret == 0) {
		args[0] = command.name;
		args[1] = command.args;
		printf("%s\n", command.prompt);
		execvp(args[0], args);

	} else {
		wait(NULL);
		printf("\n");
		gettimeofday(endTime, NULL);
		totalTime = endTime->tv_usec - startTime->tv_usec;
	}

	return totalTime;
}

void printCharacterCommands() {
	printf("   a. add command  : Adds a new command to the menu.\n");
	printf("   c. change directory : Changes process working directory\n");
	printf("   e. exit : Leave Mid-Day Commander\n");
	printf("   p. pwd : Prints working directory\n");
}


int loadInitialCommands(ll *commands) {
	cmd whoami = setupCommand("whoami", NULL, "-- Who am I? --",
			"Prints out the result of the whoami command", 0);
	int didSucceed = addCmd(commands, whoami);


	cmd last = setupCommand("last", NULL, "-- Last Logins --",
			"Prints out the result of the last command", 0);
	didSucceed &= addCmd(commands, last);


	cmd ls = setupCommand("ls", NULL, "-- Directory Listing --",
			"Prints out the result of a listing on a user-specified path", 1);
	didSucceed &= addCmd(commands, ls);

	return didSucceed;

}
