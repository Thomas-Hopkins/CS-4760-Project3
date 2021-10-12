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

void lock(int num);
void unlock(int num);

// Private helper function to get shared memory id
int getsharedmem() {
	key_t key;

	// Get numeric key of shared memory file
	key = ftok(SHM_FILE, 0);
	if (key == -1) return -1;

	// Get shared memory id from the key
	return shmget(key, sizeof(struct license_mem), 0644 | IPC_CREAT);
}

// Get the semaphore id for use with this license manager
int getsemaphores() {
	key_t key;
	// Get numeric key of shared memory file
	key = ftok(SHM_FILE, 1);
	if (key == -1) return -1;

	// Get access to semaphore set (2 semaphores)
	if ((sem_id = semget(key, 2, 0644 | IPC_CREAT)) == -1) return -1;

	return 0;
}

// Initialize the semaphores to their initial value of 1
int initsemaphores() {
	union semun arg;
	arg.val = 1;
	// Set the values for the two semaphores
	if ((semctl(sem_id, 0, SETVAL, arg)) == -1) return -1;
	if ((semctl(sem_id, 1, SETVAL, arg)) == -1) return -1;
	return 0;
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

	return 0;
}

// Tries to get a license if available, blocks if unavailable. Returns -1 if failed to get shared memory.
int getlicense() {
	lock(0);
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

	unlock(0);
	return 0;
}

// Public function to be called after a process has finished executing
int returnlicense() {
	int ret;
	lock(0);
	fprintf(stderr, "%d: Returning a license.\n", getpid());
	ret = shared_mem->nlicenses++;
	unlock(0);
	return ret;
}

// Intended to be called only once. Child processes should call attachsharedmem()
// and getsemaphores() separately so as not to override the intial values of these
// shared constructs.
int initlicense() {
	// If shared memory is not initialized do so
	if (shared_mem == NULL) {
		if (attachsharedmem() == -1) return -1;
	}
	// If semaphores are not initalized do so.
	if (sem_id < 0) {
		if (getsemaphores() == -1) return -1;
		if (initsemaphores() == -1) return -1;
	}
	shared_mem->nlicenses = 0;

	return 0;
}

// Public function to increase number of available licenses
int addtolicense(int n) {
	int ret;
	lock(0);
	shared_mem->nlicenses += n;
	ret = shared_mem->nlicenses;
	unlock(0);
	return ret;
}

// Public function to decrease number of available licenses
int removelicenses(int n) {
	lock(0);
	while (n > 0) {
		// Return error if no more licenses to remove
		if (shared_mem->nlicenses == 0) return -1;
		// try to get a license to remove
		getlicense();
		// return and remove it
		returnlicense();
		shared_mem->nlicenses--;
	}
	unlock(0);
	return 0;
}

// Public function to destroy license and shared memory
int destlicense() {
	int mem_id = getsharedmem();

	if (mem_id == -1) return -1;
	// Remove semaphore
	if ((semctl(sem_id, 0, IPC_RMID)) == -1) return -1;
	// Remove shared memory
	return shmctl(mem_id, IPC_RMID, NULL);
}

// private function to block use of critical resource until it has been unlocked
void lock(int num) {
	struct sembuf myop[1];
	myop->sem_num = (short)num;
	myop->sem_op = (short)-1;
	myop->sem_flg = (short)0;
	if ((semop(sem_id, myop, 1)) == -1) perror("Could not lock!");
	fprintf(stderr, "%d: Got lock on critical resource %d\n", getpid(), num);
}

// Private function to unlock critical resource
void unlock(int num) {
	struct sembuf myop[1];
	myop->sem_num = (short)num;
	myop->sem_op = (short)1;
	myop->sem_flg = (short)0;
	if ((semop(sem_id, myop, 1)) == -1) perror("Could not unlock!");
	fprintf(stderr, "%d: Released lock on critical resource %d\n", getpid(), num);
} 

// Public function to log a message (critical resource)
void logmsg(const char* msg) {
	lock(1);

	addmsg(non_type, msg);
	savelog(LOG_FILE);
	clearlog();

	unlock(1);
}

