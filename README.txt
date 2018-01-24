Alexander Wurts
CS 3013 Project 1
January 2018


Run Instructions:
	1) Navigate to main folder: ajwurts_proj1
	2) type: make all
	3) 3 executables are now available, mc0, mc1 and mc2
	4) Run them like any normal executable
	
Run Instructions with file as input:
	1) Do steps 1-3 from above then type the following
		-: ./mc2 < [input file name]
	2) In place of [input file name] enter the file name
	
	Note: Test input files are located in the sub-folder test_input


Testing:
	-All tests used are in the folder test_input
	-All outputs from tests are in the folder test_input/output
	

Commenting:
	- Only mc2 is fully commented but hopefully
	
Data Structure for user added commands and background processes:
	struct linkedlist {
		ll *next;
		cmd cmd;
	};
	
	struct command {
		char* name; // Command name
		char** args; // Command arguments in list form
		char* prompt; // Printed right before program is run
		char* desc; // Printed in the start
		int isBckg; // Identifies if process runs in background or not
	};
	
	   I used a linked list to store the commands and iterate through them
	every time they needed to be printed at the start of the program. When
	a user added their own command it was added to the end of the list and
	assigned the appropriate ID to be run. The linked list struct contains
	a command struct that is used for store the data for each command.
	
	I used a grandchild to run every process and a corresponding child to
	wait for each grandchild to finish. If it was not a background
	process the parent process would wait for the child to terminate before
	continuing. If it was a background process the parent process would
	not wait and move on to the next command allowing the child process to
	handle the running grandchild.
	
		To keep track of background processes I used the same linked list.
	Everytime a process ran in the background it would be added to a list
	of processes. In order to signal the parent program when a grandchild
	finished a loop is run everytime a use enters a command to handle any
	zombie children and remove them from the background process list.
	
	Example Forking for every process
	
	int ret = fork();
	
	if (ret == 0) {
		// Child Process
		int ret2 = fork();
		
		if (ret2 == 0) {
			// Grandchild Process
			run process;
		} else {
			wait for process
			print statistics
		}
	
		exit(1); // Exit child process
		
	} else if (not background process) {
		wait for child to finish and then continue main program
		
	} else { // Assumes process is background process
		// does not wait for child process to finish
		adds to list of background commands
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	