#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

void help() {
	printf("Runsim help.\n");
	printf("\n");
	printf("[n]\tNumber of processes to run.\n");
	printf("[-h]\tShow this help dialogue.\n");
	printf("\n");
}

int docommand(char* command) {
	char* program = strtok(command, " ");
	char* cmd1 = strtok(NULL, " ");
	char* cmd2 = strtok(NULL, " ");
	return execl(program, cmd1, cmd2, NULL);
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
				return EXIT_SUCCESS;
			case '?':
				// Let getopt handle error msg
				return EXIT_FAILURE;
		}
	}
	
	// process non-flag options (number of processes)
	do {
		// Make sure we got a num processes specified
		if (optind >= argc) {
			errno = EINVAL;
			fprintf(stderr, "%s: ", exe_name);
			perror("Did not get number of processes as arg");
			return EXIT_FAILURE;
		}
		
		num_apps = atoi(argv[optind++]);
		
		// error if got more than one option
		if (optind < argc) {
			errno = EINVAL;
			fprintf(stderr, "%s: ", exe_name);
			perror("Unkown option");
			return EXIT_FAILURE;
		}
		
		// error if got invalid num_apps
		if (num_apps == 0) {
			errno = EINVAL;
			fprintf(stderr, "%s: ", exe_name);
			perror("Passed invalid integer for number of processes");
			return EXIT_FAILURE;
		}
	} while (optind < argc);

	char input_buffer[MAX_CANON]; // Setup a buffer to read in text
	
	// Read in MAX_CANON chars into buffer until end
	while (fgets(input_buffer, MAX_CANON, stdin) != NULL) {
		// Get new size (in characters) of the string
		size_t input_size = strlen(input_buffer) + 1;
		// Allocate memory for this string
		char* input_text = malloc(input_size * sizeof(char));
		if (input_text == NULL) {
			fprintf(stderr, "%s: ", exe_name);
			perror("Failed to allocate memory for input");
			return EXIT_FAILURE;
		}
		
		// Copy buffer characters to string	
		strncpy(input_text, input_buffer, input_size);
		// Fork a child and have child run docommand
		if (fork() == 0) {
			if (docommand(input_text) == -1) {
				fprintf(stderr, "%s: ", exe_name);
				perror("Failed to execute child process");
			}
		}
	}
	
}
