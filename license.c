#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "license.h"
#include "config.h"
#include "lib/log.h"

extern char* exe_name;
int* nlicenses = NULL;

// Private helper function to get shared memory id
int getsharedmem(unsigned int token) {
	key_t key;

	// Get numeric key of shared memory file
	key = ftok(SHM_FILE, token);
	if (key == -1) {
		return -1;
	}

	// Get shared memory id from the key
	return shmget(key, sizeof(int), 0644 | IPC_CREAT);
}

// Public function to attach to shared memory
int attachsharedmem() {
	fprintf(stderr, "%d: Attaching shared memory...\n", getpid());
	// Get shared memory block id
	int mem_id = getsharedmem(0);

	if (mem_id == -1) {
		return -1;
	}

	// Attach shared memory to nlicenses pointer
	nlicenses = shmat(mem_id, NULL, 0);
	if (nlicenses == (int*)-1 ) {
		return -1;
	}
	return 0;
}

// Tries to get a license if available, blocks if unavailable. Returns -1 if failed to get shared memory.
int getlicense() {
	// Initialize shared memory if not available 
	if (nlicenses == NULL) {
		if (attachsharedmem() == -1) return -1;
	}

	// Block process until it has a child return which should have returned a license
	if (*nlicenses <= 0) {
		fprintf(stderr, "%d: Waiting for available license...\n", getpid());
		wait(NULL);
	}
	// Take license
	(*nlicenses)--;

	return 0;
}

// Public function to be called after a process has finished executing
int returnlicense() {
	fprintf(stderr, "%d: Returning a license.\n", getpid());
	return (*nlicenses)++;
}

// Public function to initialize number of licenses returns -1 if unsucessful
int initlicense() {
	if (nlicenses == NULL) {
		if (attachsharedmem() == -1) return -1;
	}
	*nlicenses = 0;

	return 0;
}

// Public function to increase number of available licenses
int addtolicense(int n) {
	return *nlicenses += n;
}

// Public function to decrease number of available licenses
int removelicenses(int n) {
	while (n > 0) {
		// Return error if no more licenses to remove
		if (*nlicenses == 0) return -1;
		// try to get a license to remove
		getlicense();
		// return and remove it
		returnlicense();
		(*nlicenses)--;
	}
	return 0;
}

// Public function to destroy license and shared memory
int destlicense() {
	int mem_id = getsharedmem(0);

	if (mem_id == -1) return -1;

	return shmctl(mem_id, IPC_RMID, NULL);
}

void logmsg(const char* msg) {
	addmsg(non_type, msg);
	savelog(LOG_FILE);
	clearlog();
}

