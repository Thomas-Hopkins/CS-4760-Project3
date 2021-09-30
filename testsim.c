#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "license.h"
#include "utils.h"

void help () {
	printf("testsim help\n");
	printf("\n");
	printf("t\tTime in seconds to sleep between logging calls. (Required)\n");
	printf("n\tNumber of times to output a log message.\n");
	printf("[-h]\tShow this help dialogue.\n");
	printf("\n");
}

int main(int argc, char** argv) {
	int option;
	char* exe_name = argv[0];

	while ((option = getopt(argc, argv, "hp:" )) != -1) {
		switch(option) {
			case 'h':
				help();
				return EXIT_SUCCESS;
			case '?':
				// getopt handles error 
				return EXIT_FAILURE;
		}
	}
	
	// Handle non-flag args
	if (argc - optind < 2) {
		errno = EINVAL;
		outputerror(exe_name, "Not enough arguments. See -h for help", EXIT_FAILURE);
	} 
	int sleep_t = atoi(argv[optind++]);
	int repeat = atoi(argv[optind++]);
	if (sleep_t == 0 || repeat == 0) {
		errno = EINVAL;
		outputerror(exe_name, "Invalid arguments. See -h for help", EXIT_FAILURE);
	}
	if (optind < argc) {
		errno = EINVAL;
		outputerror(exe_name, "Too many arguments. See -h for help", EXIT_FAILURE);
	}
	
	attachsharedmem();

	// Main loop
	for (int i = 1; i <= repeat; i++) {
		sleep(sleep_t);
		// Format a message to log and log it
		char message[100];
		sprintf(message, "%d\t\tIteration %d of %d",  getpid(), i, repeat);
		logmsg(message);
	}
}
