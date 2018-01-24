// mc1.c
// by Alexander Wurts
// January 2018
// For class CS3013 Project 1

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

#include "linkedlist.h"


int loadInitialCommands(ll*);
void printCharacterCommands();
void runCommand(cmd);
void printStats(struct timeval*);

int main(int argc, char* argv[]) {
	// initialize commands
	ll *commands = getLL();

	// load initial commands into commands list
	loadInitialCommands(commands);

	// Allocate and Initialize Input Buffer and Directory String
	char* input = malloc(sizeof(char) * 130);
	char* directory = malloc(sizeof(char) * 128);
	directory = strdup(argv[0]); // Retrieves directory name from argv
	directory[strlen(argv[0]) - 4] = 0; // Removes processes name from directory


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
			runCommand(command);

		} else {
			switch (input[0]) {
				case 'a': {
					printf("-- Add a Command --\n");
					printf("Command to add?: ");
					scanf("%[^\n]%*c", input);
					cmd temp = setupUserCommand(input);
					addCmd(commands, temp);
					printf("Okay, added with ID %d\n\n", size(commands) - 1);
					break;
				}
				case 'c':
					printf("-- Change Directory --\n");
					printf("Enter new directory: ");
					fgets(input, sizeof(char) * 128, stdin); // New Directory Name
					input[strlen(input) - 1] = 0; // Removes \n from end of input
					int ret = chdir(input); // Changes directory
					printf("\n");


					if (ret == 0) { // If Succeeded

						if (strstr(input, "..") && strlen(directory) > 4) { // If user entered .. to move out one folder
							char *temp = directory;
							temp += strlen(temp);
							char c;
							// While loop to find final / and set that place 0
							while ((c = *(temp)) != '/')
								temp--;
							*temp = 0;

						} else { // If user gave a normal file name saves it
							directory = strdup(input);
						}
						printf("Directory Change Successful\n");

					} else { // Failed to change directory
						printf("Directory Change Failed\n");
					}
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
					printf("Invalid command please try again.\n\n");
				}

			}
		}
	}
	return 0;
}

void runCommand(cmd command) {

	if (!strcmp(command.name, "ls")) { // Handles user input for ls command

			// Allocates Buffers
			char *inputBuffer = malloc(sizeof(char) * 128);
			char *argBuffer = malloc(sizeof(char) * 128);
			printf("\n-- Directory Listing --\n");

			// Gets user input from stdin for arguments
			printf("Arguments?: ");
			fgets(inputBuffer, sizeof inputBuffer, stdin);
			if (*inputBuffer == '\n')
				; //do nothing
			else {
				// Remove \n from end of input string
				*(inputBuffer + strlen(inputBuffer) - 1) = 0;
				// Appends to end of argbuffer for proper argument passing
				strcat(argBuffer, inputBuffer);
			}

			// Get  user input for path
			printf("Path?: ");
			fgets(inputBuffer, sizeof inputBuffer, stdin);
			if (*inputBuffer == '\n')
					inputBuffer = NULL;
			else {
				// Remove \n from end
				*(inputBuffer + strlen(inputBuffer) - 1) = 0;
				// Add stuff to inputBuffer then transfers to argBuffer for condition check below
				if (*argBuffer != '\0')
					strcat(inputBuffer, " ");
				strcat(inputBuffer, argBuffer);
				free(argBuffer);
				argBuffer = inputBuffer;
			}

			if (inputBuffer != NULL || strlen(argBuffer) >= 2) {
				// Parses arguments from string
				command.args = parseArgString(argBuffer);
			}
			// Sets args[0] to command name for when processes executes
			command.args[0] = strdup(command.name);

			printf("\n\n");
			free(inputBuffer);
			// Concat inputed args tp create command args.
		}


		struct timeval start_time, end_time, sub;

		gettimeofday(&start_time, NULL);
		int ret = fork();
		if (ret < 0) {
			fprintf(stderr, "fork failed\n");
			exit(1);
		} else if (ret == 0) {

			int ret2 = fork();
			if (ret2 == 0) {
				// Prints prompt and runs command
				printf("%s\n", command.prompt);
				execvp(command.args[0], command.args);
			} else {
				waitpid(ret2, 0, 0);
				gettimeofday(&end_time, NULL);

				timersub(&end_time, &start_time, &sub);
				printStats(&sub);

			}
			exit(ret2);
		} else
			waitpid(ret, 0, 0); // Waits for returning process

}

void printCharacterCommands() {
	printf("   a. add command  : Adds a new command to the menu.\n");
	printf("   c. change directory : Changes process working directory\n");
	printf("   e. exit : Leave Mid-Day Commander\n");
	printf("   p. pwd : Prints working directory\n");
}

void printStats(struct timeval *time) {
	struct rusage *usage = malloc(sizeof(struct rusage));
		usage = malloc(sizeof(struct rusage));

		if (getrusage(RUSAGE_CHILDREN, usage) != 0) {
			printf("Error Could Not get usage: %d", errno);
		} else {

			long milli = (time->tv_sec * 1000) + (time->tv_usec / 1000);

			printf("\n--- Statistics ---\n");
			printf("Elapsed time: %lu millisecond(s)\n", milli);
			printf("Page Faults: %lu\n", usage->ru_majflt);
			printf("Page Faults (reclaimed): %lu\n\n", usage->ru_minflt);
			free(usage);
		}
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
