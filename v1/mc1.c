#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "linkedlist.h"

typedef struct __stats {
	long prev_ru_minflt;
	long prev_ru_majflt;
	long current_minflt;
	long current_majflt;
	long time;
} stats;
int loadInitialCommands(ll*);
void printCharacterCommands();
long runCommand(cmd);
stats *printStats(stats*);

int main(int argc, char* argv[]) {
	ll *commands = getLL();

	loadInitialCommands(commands);
	stats *cmdStats = malloc(sizeof(stats));
	cmdStats->prev_ru_minflt = 0; cmdStats->prev_ru_majflt = 0;
	char* input = malloc(sizeof(char) * 130);
	char* directory = malloc(sizeof(char) * 128);
	directory = argv[0];
	argv[0][strlen(argv[0]-5)] = 0;
	printf("%s", input);

	while (1) {
		// hard coded - a, c, e, pwd
		// part of commmands 0, 1, 2, ...

		// Print current commands:
		printf("===== Mid-Day Commander, v1 =====\n");
		printf("G'Day, Commander! What command would you like to run?\n");
		printWithEnumeration(commands);
		printCharacterCommands();
		printf("Option?: ");


		char* done = fgets(input, sizeof input, stdin);
		if (done == NULL) {
			input[0] = 'e';
		}

		printf("\n");

		if (48 <= input[0] && input[0] <= 57) {
			int i = atoi(input);
			cmd command = getIthFromLL(commands, i);
			cmdStats->time = runCommand(command);
			cmdStats = printStats(cmdStats);
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
					printf("Logging you out, Commander.\n");
					exit(0);
					break;
				}

				default: {
					printf("Invalid command please try again\n\n");
					printf("%d", (int)input[0]);
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


	if (!strcmp(command.name, "ls")) {
		char *inputBuffer = malloc(sizeof(char) * 128);
		char *argBuffer = malloc(sizeof(char) * 128);
		printf("\n-- Directory Listing --\n");
		printf("Arguments?: ");
		fgets(inputBuffer, sizeof inputBuffer, stdin);
		if (*inputBuffer == '\n')
			; //do nothing
		else {
			*(inputBuffer + strlen(inputBuffer) - 1) = 0;
			strcat(argBuffer, inputBuffer);
		}

		printf("Path?: ");
		fgets(inputBuffer, sizeof inputBuffer, stdin);
		if (*inputBuffer == '\n')
				inputBuffer = NULL;
		else {
			*(inputBuffer + strlen(inputBuffer) - 1) = 0;
			strcat(inputBuffer, " ");
			strcat(inputBuffer, argBuffer);
			free(argBuffer);
			argBuffer = inputBuffer;
		}

		if (inputBuffer != NULL) {
			command.args = parseArgString(argBuffer);
		}
		command.args[0] = strdup(command.name);
		//"%[^\n]%*c"
		printf("\n\n");
		free(inputBuffer);
		// Concat inputed args tp create command args.
	}

	gettimeofday(startTime, NULL);
	int ret = fork();
	if (ret < 0) {
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (ret == 0) {
		printf("%s\n", command.prompt);
		execvp(command.args[0], command.args);
	} else {
		wait(NULL);
		gettimeofday(endTime, NULL);
		printf("\n");
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

stats *printStats(stats *cmdStats) {
	struct rusage *usage;
	usage = malloc(sizeof(struct rusage));

	getrusage(RUSAGE_CHILDREN, usage);
	cmdStats->current_minflt = usage->ru_minflt - cmdStats->prev_ru_minflt;
	cmdStats->current_majflt = usage->ru_majflt - cmdStats->prev_ru_majflt;

	cmdStats->prev_ru_minflt += cmdStats->current_minflt;
	cmdStats->prev_ru_majflt += cmdStats->current_majflt;

	printf("\n--- Statistics ---\n");
	printf("Elapsed time: %lu millisecond(s)\n", cmdStats->time / 1000);
	printf("Page Faults: %lu\n", cmdStats->current_majflt);
	printf("Page Faults (reclaimed): %lu\n\n", cmdStats->current_minflt);
	free(usage);
	return cmdStats;
}

int loadInitialCommands(ll *commands) {
	cmd whoami = setupCommand("whoami", "-- Who am I? --",
			"Prints out the result of the whoami command");
	int didSucceed = addCmd(commands, whoami);

	cmd last = setupCommand("last", "-- Last Logins --",
			"Prints out the result of the last command");
	didSucceed &= addCmd(commands, last);

	cmd ls = setupCommand("ls", "-- Directory Listing --",
			"Prints out the result of a listing on a user-specified path");
	didSucceed &= addCmd(commands, ls);

	return didSucceed;
}
