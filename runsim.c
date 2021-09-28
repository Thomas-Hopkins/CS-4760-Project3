#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

#include "license.h"
#include "config.h"
#include "utils.h"

static int num_children;
static pid_t children[MAX_PROCESSES]; 

void help() {
	printf("Runsim help.\n");
	printf("\n");
	printf("n\tNumber of processes to run. (Required)\n");
	printf("[-h]\tShow this help dialogue.\n");
	printf("\n");
}

int removechild(pid_t pid) {	
	// Remove pid from children list (slow linear search - but small list so inconsequential)
	for (int i = 0; i < num_children; i++) {
		if (children[i] == pid) {
			// If match, replace pid with last pid and decrement list
			children[i] = children[--num_children];
			children[num_children] = 0;
			return 0;
		}
	}
	return -1;
}

void cleanup() {
	return;
}

void signal_handler(int signum) {
	// Issue messages	
	if (signum == SIGINT) {
		fprintf(stderr, "\nRecieved SIGINT signal interrupt, terminating children.\n");
	}
	else if (signum == SIGALRM) {
		fprintf(stderr, "\nProcess execution timeout. Failed to finish in %d seconds.\n", MAX_TIMEOUT);
	}

	// Kill all children processes back to front
	while (num_children > 0) {		
		kill(children[num_children - 1], SIGKILL);
		removechild(children[num_children - 1]);
		children[num_children--] = 0;
	}

	// Cleanup license object (remove shared memory)
	destlicense();

	if (signum == SIGINT) exit(EXIT_SUCCESS);
	if (signum == SIGALRM) exit(EXIT_FAILURE);
}

// Issue a execl call
int docommand(char* command) {
	char* program = strtok(command, " ");
	char* cmd1 = strtok(NULL, " ");
	char* cmd2 = strtok(NULL, " ");
	return execl(program, program, cmd1, cmd2, NULL);
}

int main(int argc, char** argv) {
	int option;
	int num_apps;
	char* exe_name = argv[0];
	
	// Process args
	while ((option = getopt(argc, argv, "h")) != -1) {
		switch(option) {
			case 'h':
				help();
				exit(EXIT_SUCCESS);
			case '?':
				// Let getopt handle error msg
				exit(EXIT_FAILURE);
		}
	}
	
	// process non-flag options (number of processes)
	do {
		// Make sure we got a num processes specified
		if (optind >= argc) {
			errno = EINVAL;
			outputerror(exe_name, "Did not get number of processes as arg", EXIT_FAILURE);
		}
		
		num_apps = atoi(argv[optind++]);
		if (num_apps > MAX_PROCESSES) {
			errno = EINVAL;
			outputerror(exe_name, "Too many processes requested. Falling back to max", -1);
			num_apps = MAX_PROCESSES;
		}
		
		// error if got more than one option
		if (optind < argc) {
			errno = EINVAL;
			outputerror(exe_name, "Unkown option", EXIT_FAILURE);
		}
		
		// error if got invalid num_apps
		if (num_apps == 0) {
			errno = EINVAL;
			outputerror(exe_name, "Passed invalid integer for number of processes", EXIT_FAILURE);
		}
	} while (optind < argc);
	
	//Intialize license object
	if (initlicense() == -1) {
		errno = EINVAL;
		outputerror(exe_name, "Failed to initialize shared memory block", EXIT_FAILURE);
	}
	fprintf(stderr, "apps: %d\n", num_apps);
	addtolicense(num_apps);


	// Setup signal handlers
	signal(SIGINT, signal_handler);
	signal(SIGALRM, signal_handler);

	// Terminate if children do not finish in timeout time	
	alarm(MAX_TIMEOUT);

	char input_buffer[MAX_CANON]; // Setup a buffer to read in text
	
	// MAIN LOOP //
	// Read in max characters per line into buffer then execute their command
	while (fgets(input_buffer, MAX_CANON, stdin) != NULL) {
		// Pid holder variable
		pid_t pid; 

		// GET INPUT FOR COMMAND //
		// Get new size (in characters) of the string
		size_t input_size = strlen(input_buffer) + 1;
		// Allocate memory for this string
		char* input_text = malloc(input_size * sizeof(char));
		if (input_text == NULL) {
			outputerror(exe_name, "Failed to allocate memory for input", EXIT_FAILURE);
		}
		
		// Copy buffer characters to string	
		strncpy(input_text, input_buffer, input_size);

		// REQUEST LICENSE //
		getlicense();

		// RUN COMMAND //
		// Fork children and have children run docommand
		pid = fork();
		if (pid == 0) {
			// We are a child, execute command
			if (docommand(input_text) == -1) {
				outputerror(exe_name, "Failed to execute child process", EXIT_FAILURE);
			}
		}
		else {
			// We are parent, append this pid to children
			children[num_children++] = pid;
			free(input_text);
		}

		// See if any children have returned, if so return a license and remove child
		pid = waitpid(-1, NULL, WNOHANG);
		if (pid > 0) {
			returnlicense();
			removechild(pid);
		} 
	}

	// Wait until all children finish
	while (num_children > 0) {
		pid_t pid = wait(NULL);
		returnlicense();
		removechild(pid);
	}
	
	// Remove shared memory
	destlicense();
}
