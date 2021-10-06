#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#include "license.h"
#include "config.h"
#include "lib/log.h"

struct license_mem* shared_mem = NULL;
static int sem_id = -1;

void lock();
void unlock();

// Private helper function to get shared memory id
int getsharedmem() {
	key_t key;

	// Get numeric key of shared memory file
	key = ftok(SHM_FILE, 0);
	if (key == -1) return -1;

	// Get shared memory id from the key
	return shmget(key, sizeof(struct license_mem), 0644 | IPC_CREAT);
}

int initsemaphores() {
	key_t key;
	int sem_id = 0;

	//TODO: Implement semaphore initialization

	return sem_id;
}

// Public function to attach to shared memory
int attachsharedmem() {
	fprintf(stderr, "%d: Attaching shared memory...\n", getpid());
	// Get shared memory block id 
	int mem_id = getsharedmem();
	if (mem_id == -1) return -1;

	// Attach shared memory to struct pointer
	shared_mem = shmat(mem_id, NULL, 0);
	if (shared_mem == (void*)-1 ) return -1;

	sem_id = initsemaphores();
	if (sem_id < 0) return -1;

	return 0;
}

// Tries to get a license if available, blocks if unavailable. Returns -1 if failed to get shared memory.
int getlicense() {
	lock();
	// Initialize shared memory if not available 
	if (shared_mem == NULL) {
		if (attachsharedmem() == -1) return -1;
	}

	// Block process until it has a child return which should have returned a license
	if (shared_mem->nlicenses <= 0) {
		fprintf(stderr, "%d: Waiting for available license...\n", getpid());
		wait(NULL);
	}
	// Take license
	shared_mem->nlicenses--;

	unlock();
	return 0;
}

// Public function to be called after a process has finished executing
int returnlicense() {
	int ret;
	lock();
	fprintf(stderr, "%d: Returning a license.\n", getpid());
	ret = shared_mem->nlicenses++;
	unlock();
	return ret;
}

// Public function to initialize number of licenses returns -1 if unsucessful
int initlicense() {
	if (shared_mem == NULL) {
		if (attachsharedmem() == -1) return -1;
	}
	shared_mem->nlicenses = 0;

	return 0;
}

// Public function to increase number of available licenses
int addtolicense(int n) {
	int ret;
	lock();
	shared_mem->nlicenses += n;
	ret = shared_mem->nlicenses;
	unlock();
	return ret;
}

// Public function to decrease number of available licenses
int removelicenses(int n) {
	lock();
	while (n > 0) {
		// Return error if no more licenses to remove
		if (shared_mem->nlicenses == 0) return -1;
		// try to get a license to remove
		getlicense();
		// return and remove it
		returnlicense();
		shared_mem->nlicenses--;
	}
	unlock();
	return 0;
}

// Public function to destroy license and shared memory
int destlicense() {
	int mem_id = getsharedmem();

	if (mem_id == -1) return -1;

	return shmctl(mem_id, IPC_RMID, NULL);
}

// private function to block use of critical resource until it has been unlocked
void lock() {
	//TODO: semaphore lock with semop()
	return;
}

// Private function to unlock critical resource
void unlock() {
	//TODO: semaphore unlock with semop()
	return;
} 

// Public function to log a message (critical resource)
void logmsg(const char* msg) {
	lock(getpid() % MAX_PROCESSES);

	addmsg(non_type, msg);
	savelog(LOG_FILE);
	clearlog();

	unlock(getpid() % MAX_PROCESSES);
}

