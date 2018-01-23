#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "linkedlist2.h"


// Struct used to hold information about stats
typedef struct __stats {
	long prev_ru_minflt;
	long prev_ru_majflt;
	long current_minflt;
	long current_majflt;
	long time;
} stats;


void loadInitialCommands(ll*); // Load initial commands and add them to commands list
void printCharacterCommands(); // Print commands from commands list
long runCommand(cmd, ll**); // Runs given command and uses ll* depending on if running in background
ll* waitForBKP(ll*, stats*); // Waits for processes running in the background
void printStats(stats*); // print stats from stats struct

int main(int argc, char* argv[]) {
	// initialize commands and background commands
	ll *commands = getLL();
	ll *bckgCmds = getLL();

	// load initial commands into commands list
	loadInitialCommands(commands);

	// Allocate and initialize command stats
	stats *cmdStats = malloc(sizeof(stats));
	cmdStats->prev_ru_minflt = 0; cmdStats->prev_ru_majflt = 0;

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

			cmdStats->time = runCommand(command, &bckgCmds); // Runs command
			if (!command.isBckg) // Prints stats if not background
				printStats(cmdStats);

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
					while (current != NULL) {
						if (current->cmd.pid != 0) // Needed for edge case when bckgCmds is 0;
							printf("[%d]: PID: %d, Name: %s\n", i++,
														   current->cmd.pid,
														   current->cmd.name);
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
		bckgCmds = waitForBKP(bckgCmds, cmdStats);
	}

	freeLL(commands);
	freeLL(bckgCmds);
	free(input);
	free(directory);
	free(cmdStats);
	return 0;
}

// Runs given command and return time to run processes
long runCommand(cmd command, ll** bckgCmds) {

	struct timeval *startTime, *endTime;
	startTime = malloc(sizeof(struct timeval));
	endTime = malloc(sizeof(struct timeval));
	long totalTime;


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


	// Starts timer
	gettimeofday(startTime, NULL);
	int ret = fork();
	if (ret < 0) {
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (ret == 0) {
		// Prints prompt and runs command
		printf("%s\n", command.prompt);
		execvp(command.args[0], command.args);

	} else {
		if (command.isBckg) { // Background Command
			cmd temp = command;
			temp.pid = ret;
			temp.startTime = startTime;
			*bckgCmds = addCmd(*bckgCmds, temp);
		} else { // Non-Background Command
			waitpid(ret, NULL, 0); // Waits for returning process
			gettimeofday(endTime, NULL); // Stops timer


			// Calculates time difference between finish and start and returns long
			long secs = endTime->tv_sec - startTime->tv_sec;
			long usecs = 0;
			long et_usec = endTime->tv_usec;
			long st_usec = startTime->tv_usec;
			if (et_usec < st_usec) {
				usecs = (1000000 + et_usec) - st_usec;
			} else {
				usecs = et_usec - st_usec;
			}
			totalTime = (secs * 1000000) + (usecs);

			printf("\n");
		}


	}

	return totalTime;
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
void printStats(stats *cmdStats) {
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
ll* waitForBKP(ll* bckgCmds, stats* cmdStats) {
	struct timeval *endTime = malloc(sizeof(struct timeval));
	ll* c_cmd = bckgCmds; // Used to read down the list without changing bckgCmds
	siginfo_t info;
	// Waits for all running commands
	// If command is not already finished when this runs starts main program again
	while (c_cmd != NULL){
		info.si_pid = 0; // Sets pid to zero to know if it waited for a process or not

		waitid(P_ALL, c_cmd->cmd.pid, &info, WEXITED | WNOHANG);

		if (info.si_pid != 0) {
			gettimeofday(endTime, NULL); // Stops Timer

			// Calculates difference in time
			long secs = endTime->tv_sec - c_cmd->cmd.startTime->tv_sec;
			long usecs = 0;
			long et_usec = endTime->tv_usec;
			long st_usec = c_cmd->cmd.startTime->tv_usec;
			if (et_usec < st_usec) {
				usecs = (1000000 + et_usec) - st_usec;
			} else {
				usecs = et_usec - st_usec;
			}
			long milli = (secs * 100000) + (usecs);

			cmdStats->time = milli;

			printf("--Job Complete --\n"); // Prints out job completion, ID and proces name
			printf("Process ID: %d\n", (int)info.si_pid);
			printf("Command Name: %s\n", c_cmd->cmd.name);
			printStats(cmdStats); // Prints Stats

			// Removes from background commands list
			bckgCmds = delPID(bckgCmds, (int)info.si_pid);

		}

		// Moves on to next background command
		c_cmd = c_cmd->next;
	}
	free(endTime);
	return bckgCmds;
}
