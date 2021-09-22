#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

void help() {
	printf("Runsim help.\n");
	printf("\n");
	printf("[n]\tNumber of processes to run.\n");
	printf("[-h]\tShow this help dialogue.\n");
	printf("\n");
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
	
	// TODO: implement licensing
	printf("arg: %d\n", num_apps);
}
