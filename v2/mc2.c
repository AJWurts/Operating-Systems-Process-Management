#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

#include "linkedlist2.h"


// Struct used to hold information about stats



void loadInitialCommands(ll*); // Load initial commands and add them to commands list
void printCharacterCommands(); // Print commands from commands list
ll* catchBackgroundProcesses(ll* bckgCmds, int *numBckgCmds); // Waits for processes running in the background
void runCommand(cmd command, ll** bckgCmds, int *numBckgCmds);
void printStats(struct timeval*);// print stats from stats struct

int main(int argc, char* argv[]) {
	// initialize commands and background commands
	ll *commands = getLL();
	ll *bckgCmds = getLL();
	int numBckgCmds = 0;

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

		// Get user input
		char* done = fgets(input, sizeof input, stdin);
		if (done == NULL) { // If no input assume user wants to quit program
			input[0] = 'e';
		}

		printf("\n");
		// Checks to see if input is a number or a character
		// If its a number it looks for the corresponding command and runs it
		// If its a character it assumes its a built in command
		if (48 <= input[0] && input[0] <= 57) {
			int i = atoi(input); // Turns input into int to find right command
			cmd command = getIthFromLL(commands, i); // gets command

			runCommand(command, &bckgCmds, &numBckgCmds); // Runs command


		} else {
			switch (input[0]) {
				case 'a': { // Add Command Case
					printf("-- Add a Command --\n");
					printf("Command to add?: ");
					scanf("%[^\n]%*c", input); // Input command
					char* name = strdup(input); // saves input to be saved as name for later


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

					// creates command and adds it to command list
					cmd temp = setupUserCommand(input, "User added command", isBckg);
					temp.name = name;
					addCmd(commands, temp);
					printf("Okay, added with ID %d\n\n", size(commands) - 1);
					break;
				}
				case 'c': // Change Working Directory
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
				case 'p': { // Print Current Working Directory
					printf("-- Current Directory --\nDirectory: ");
					printf("%s\n\n", directory);
					break;
				}
				case 'e': { // Logout and exit program
					printf("Logging you out, Commander.\n");
					exit(0);
					break;
				}
				case 'r': { // Prints current running processes
					ll* current = bckgCmds;
					int i = 0;
					printf("-- Background Processes --\n");
					while (current != NULL && current->cmd.name != NULL) {
							printf("[%d]: Name: %s\n", i++, current->cmd.name);
						current = current->next;
					}
					printf("\n");
					break;
				}

				default: { // Invalid Command
					printf("Invalid command please try again\n\n");
				}
			}
		}
		bckgCmds = catchBackgroundProcesses(bckgCmds, &numBckgCmds);
	}

	freeLL(commands);
	freeLL(bckgCmds);
	free(input);
	free(directory);
	return 0;
}

// Runs given command and return time to run processes
void runCommand(cmd command, ll** bckgCmds, int *numBckgCmds) {

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

			if (command.isBckg == 1) {
				printf("-- Job Complete [%d]--\n", *numBckgCmds);
				printf("Process ID: %d\n", ret2);
				printf("[%d] %d\n", *numBckgCmds, ret2);

			}
			timersub(&end_time, &start_time, &sub);
			printStats(&sub);

		}
		exit(ret2);
	} else if (command.isBckg != 1) {
		waitpid(ret, 0, 0); // Waits for returning process
	} else {
		(*numBckgCmds)++;
		*bckgCmds = addCmd(*bckgCmds, command);
	}
}

// Prints character commands
void printCharacterCommands() {
	printf("   a. add command  : Adds a new command to the menu.\n");
	printf("   c. change directory : Changes process working directory\n");
	printf("   e. exit : Leave Mid-Day Commander\n");
	printf("   p. pwd : Prints working directory\n");
	printf("   r. running processes : Print list of running processes\n");
}

// Prints stats for most recently run process
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

// Adds inital commands to the commands ll
void loadInitialCommands(ll *commands) {
	cmd whoami = setupCommand("whoami", "-- Who am I? --",
			"Prints out the result of the whoami command");
	addCmd(commands, whoami);

	cmd last = setupCommand("last", "-- Last Logins --",
			"Prints out the result of the last command");
	addCmd(commands, last);

	cmd ls = setupCommand("ls", "-- Directory Listing --",
			"Prints out the result of a listing on a user-specified path");
	addCmd(commands, ls);


}


// Waits for background processes
ll* catchBackgroundProcesses(ll* bckgCmds, int *numBckgCmds) {
	ll* c_cmd = bckgCmds; // Used to read down the list without changing bckgCmds
	siginfo_t info;
	// Waits for all running commands
	// If command is not already finished when this runs starts main program again
	while (c_cmd != NULL){
		info.si_pid = 0; // Sets pid to zero to know if it waited for a process or not

		waitid(P_ALL, c_cmd->cmd.pid, &info, WEXITED | WNOHANG);

		if (info.si_pid != 0) {

			bckgCmds = delCmd(bckgCmds, c_cmd->cmd);
			(*numBckgCmds)--;

		}

		// Moves on to next background command
		c_cmd = c_cmd->next;
	}
	
	return bckgCmds;
}
