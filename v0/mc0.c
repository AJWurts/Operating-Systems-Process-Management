#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>



int main(int argc, char *argv[]) {
	int input;
	long totalTime, current_minflt, current_majflt;
	long prev_ru_minflt = 0, prev_ru_majflt = 0;
	struct timeval *startTime, *endTime;
	struct rusage *usage;
	startTime = malloc(sizeof(struct timeval));
	endTime = malloc(sizeof(struct timeval));
	usage = malloc(sizeof(struct rusage));


	while (1) {
		printf("===== Mid-Day Commander, v0 =====");
		printf("G'Day, Commander! What command would you like to run?\n");
		printf("\t0. whoami\t Prints out the result of the whoami command\n");
		printf("\t1. last\t Prints out the result of the last command\n");
		printf("\t2. ls\t Prints out the result of a listing on a user-specifed path\n");
		printf("Option?: ");
	
		scanf("%d", &input);
		printf("\n");



		if (input == 0) {
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
				int whoami = wait(NULL);
				gettimeofday(endTime, NULL);
				totalTime = endTime->tv_usec - startTime->tv_usec;
			}
		} else if (input == 1) {
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
				int last = wait(NULL);
				gettimeofday(endTime, NULL);
				printf("\n");
				totalTime = endTime->tv_usec - startTime->tv_usec;
			}

		} else if (input == 2) {
			char *args[3];
			args[2] = malloc(sizeof(char) * 100);
			args[1] = malloc(sizeof(char) * 100);
			args[0] = strdup("ls");
			printf("\n-- Directory Listing --\n");
			printf("Arguments?: ");
			scanf("%s", args[1]);
			printf("Path?: ");
			scanf("%s", args[2]);
			printf("\n\n");
			args[3] = NULL;
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
			printf("Invalid input. Please only type 0, 1, or 2.\n");
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
