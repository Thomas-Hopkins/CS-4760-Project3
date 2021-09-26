#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "license.h"

void help () {
	printf("testsim help\n");
}

int main(int argc, char** argv) {
	int option;
	char* exe_name = argv[0];

	while ((option = getopt(argc, argv, "h" )) != -1) {
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
		fprintf(stderr, "%s: ", exe_name);
		perror("Not enough arguments. See -h for help.");
		return EXIT_FAILURE;
	} 
	int sleep_t = atoi(argv[optind++]);
	int repeat = atoi(argv[optind++]);
	if (sleep_t == 0 || repeat == 0) {
		errno = EINVAL;
		fprintf(stderr, "%s: ", exe_name);
		perror("Invalid arguments. See -h for help.");
		return EXIT_FAILURE;
	}
	if (optind < argc) {
		errno = EINVAL;
		fprintf(stderr, "%s: ", exe_name);
		perror("Too many arguments. See -h for help.");	
	}
	
	// Main loop
	for (int i = 1; i <= repeat; i++) {
		sleep(sleep_t);
		//TODO: Call logmsg 
		printf("TIME\t%d\tIteration %d of %d\n", getpid(), i, repeat);
	}
}
