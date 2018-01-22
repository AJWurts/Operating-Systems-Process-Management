#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "linkedlist2.h"
#include "background_processes.h"

typedef struct __stats {
	long prev_ru_minflt;
	long prev_ru_majflt;
	long current_minflt;
	long current_majflt;
	long time;
} stats;

void loadInitialCommands(ll*);
void printCharacterCommands();
long runCommand(cmd, ll*);
ll* waitForBKP();
void printStats(stats*);

int main(int argc, char* argv[]) {
	ll *commands = getLL();
	ll *bckgCmds = getLL();

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
		if (done == NULL) { // If no input assume user wants to quit program
			input[0] = 'e';
		}

		printf("\n");

		if (48 <= input[0] && input[0] <= 57) {
			int i = atoi(input);
			cmd command = getIthFromLL(commands, i);
			cmdStats->time = runCommand(command, bckgCmds);
			if (!command.isBckg)
				printStats(cmdStats);
		} else {
			switch (input[0]) {
				case 'a': {
					printf("-- Add a Command --\n");
					printf("Command to add?: ");
					scanf("%[^\n]%*c", input);
					int isBckg = 0;
					if (input[strlen(input) - 1] == '&'){
						isBckg = 1;
						int i = strlen(input);
						while ((int)input[i] == 38
								||
								!(33 <= (int)input[i] && (int)input[i] <= 126)) { //  while last character is not valid character
							input[i] = '\0'; // set character to 0
							i--;
						}
					}
					cmd temp = setupUserCommand(input, "User added command", isBckg);
					addCmd(commands, temp);
					printf("Okay, added with ID %d\n\n", size(commands) - 1);
					break;
				}
				case 'c':
					printf("-- Change Directory --\n");
					printf("Enter new directory: ");
					fgets(input, sizeof input, stdin);
					input[strlen(input) - 1] = 0;
					int ret = chdir(input);
					printf("\n");
					if (ret == 0) {
						printf("Directory Change Successful\n");
					} else {
						printf("Directory Change Failed\n");
					}
					printf("\n\n");
					directory = strdup(input);
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
				case 'r': {
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

				default: {
					printf("Invalid command please try again\n\n");
//					printf("%d", (int)input[0]);
				}

			}
		}
		bckgCmds = waitForBKP(bckgCmds, cmdStats);
	}


	return 0;
}

long runCommand(cmd command, ll* bckgCmds) {

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

		if (inputBuffer != NULL || strlen(argBuffer) >= 2) {
			command.args = parseArgString(argBuffer);
		}
		command.args[0] = strdup(command.name);

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
		if (command.isBckg) {
			cmd temp = command;
			temp.pid = ret;
			temp.startTime = startTime;
			bckgCmds = addCmd(bckgCmds, temp);
		} else {
			waitpid(ret, NULL, 0);
			gettimeofday(endTime, NULL);
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

void printCharacterCommands() {
	printf("   a. add command  : Adds a new command to the menu.\n");
	printf("   c. change directory : Changes process working directory\n");
	printf("   e. exit : Leave Mid-Day Commander\n");
	printf("   p. pwd : Prints working directory\n");
	printf("   r. running processes : Print list of running processes\n");
}

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

ll* waitForBKP(ll* bckgCmds, stats* cmdStats) {
	/*int  waitid(idtype_t  idtype, id_t id, siginfo_t *infop,
       int options);
	 */
	ll* c_cmd = bckgCmds;
	siginfo_t info;
	while (c_cmd != NULL){
		info.si_pid = 0;
		waitid(P_ALL, c_cmd->cmd.pid, &info, WEXITED | WNOHANG);
		if (info.si_pid != 0) {
			struct timeval *endTime = malloc(sizeof(struct timeval));
			gettimeofday(endTime, NULL);
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
			free(endTime);
			printf("--Job Complete --\n");
			printf("Process ID: %d\n", (int)info.si_pid);
			printf("Command Name: %s\b", c_cmd->cmd.name);
			printStats(cmdStats);

			bckgCmds = delPID(bckgCmds, (int)info.si_pid);

		}
		c_cmd = c_cmd->next;
	}
	return bckgCmds;
}
