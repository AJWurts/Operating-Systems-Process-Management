#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>



char **parseArgString(char* args);

int main(int argc, char *argv[]) {
	// Initialize and Allocate Variables

	long totalTime, current_minflt, current_majflt;
	long prev_ru_minflt = 0, prev_ru_majflt = 0;
	struct timeval *startTime, *endTime;
	struct rusage *usage;
	startTime = malloc(sizeof(struct timeval));
	endTime = malloc(sizeof(struct timeval));
	usage = malloc(sizeof(struct rusage));
	char* input = malloc(sizeof(char) * 128);


	while (1) {

		// Print commands and prompt user
		printf("===== Mid-Day Commander, v0 =====");
		printf("G'Day, Commander! What command would you like to run?\n");
		printf("\t0. whoami\t Prints out the result of the whoami command\n");
		printf("\t1. last\t Prints out the result of the last command\n");
		printf("\t2. ls\t Prints out the result of a listing on a user-specifed path\n");
		printf("Option?: ");
	
		fgets(input, sizeof input, stdin);
		printf("\n");


		// Command 0: whoami
		if (*input == '0') {
			gettimeofday(startTime, NULL);
			int ret = fork();
			if (ret < 0) {
				fprintf(stderr, "fork failed\n");
				exit(1);
			} else if (ret == 0) {
				char *args[2];
				args[0] = strdup("whoami");
				args[1] = NULL;
				printf("-- Who Am I? --\n");
				execvp(args[0], args);

			} else {
				wait(NULL);
				gettimeofday(endTime, NULL);
				totalTime = endTime->tv_usec - startTime->tv_usec;
			}
		} else if (*input == '1') { // Command 1: last
			gettimeofday(startTime, NULL);
			int ret = fork();
			if (ret < 0) {
				fprintf(stderr, "fork failed\b");
				exit(1);
			} else if (ret == 0) {
				char *args[2];
				args[0] = strdup("last");
				args[1] = NULL;
				printf("-- Last Logins --\n");
				execvp(args[0], args);
			} else {
				wait(NULL);
				gettimeofday(endTime, NULL);
				printf("\n");
				totalTime = endTime->tv_usec - startTime->tv_usec;
			}

		} else if (*input == '2') { // Command 2: ls

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
			char** args = malloc(sizeof(char) * 2);
			if (inputBuffer != NULL || strlen(argBuffer) >= 2) {
				// Parses arguments from string
				args = parseArgString(argBuffer);
			} else {
				args[1] = NULL;
			}
			// Sets args[0] to command name for when processes executes
			args[0] = "ls";


			printf("\n\n");
			free(inputBuffer);
			// Concat inputed args tp create command args.
			int ret = fork();
			if (ret < 0) {
				fprintf(stderr, "fork failed\b");
				exit(1);
			} else if (ret == 0) {
				execvp(args[0], args);
			} else {
				wait(NULL);
				gettimeofday(endTime, NULL);
			}

		} else {
			printf("Invalid input. Please only type the characters 0, 1, or 2.\n");
		}
		
		// Statistics
		getrusage(RUSAGE_CHILDREN, usage);
		current_minflt = usage->ru_minflt - prev_ru_minflt;
		current_majflt = usage->ru_majflt - prev_ru_majflt;

		prev_ru_minflt += current_minflt;
		prev_ru_majflt += current_majflt;

		printf("\n--- Statistics ---\n");
		printf("Elapsed time: %lu millisecond(s)\n", totalTime / 1000);
		printf("Page Faults: %lu\n", current_majflt);
		printf("Page Faults (reclaimed): %lu\n\n", current_minflt);



	}
	
	return 0;
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
