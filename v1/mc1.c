#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "linkedlist.h"

int loadInitialCommands(ll*);

int main(int argc, char* argv[]) {
	ll *commands = getLL();

	loadInitialCommands(commands);
	char input;


	while (1) {
		// hard coded - a, c, e, pwd
		// part of commmands 0, 1, 2, ...

		// Print current commands:
		printf("===== Mid-Day Commander, v1 =====\n");
		printf("G'Day, Commander! What command would you like to run?\n");
		printWithEnumeration(commands);
		printf("Option?: ");

		scanf("%c", &input);
		printf("\n");

		if (47 < input && input < 58) {
			getIthFromLL(commands, (int)input);
		}




		break;
	}

//	if (fork() != 0) {
//		wait(NULL);
//		printf("Parent");
//	} else {
//
//	}

	return 0;
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
