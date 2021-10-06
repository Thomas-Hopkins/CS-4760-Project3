#ifndef __LICENSE_H
#define __LICENSE_H

#include <semaphore.h>
#include "config.h"

// Public member functions
int getsemaphores(); // Access semaphores 
int attachsharedmem(); // Attach to shared memory
int getlicense(); // Block until license is available
int returnlicense(); // increment available licenses
int initlicense(); // initialize license object
int addtolicense(int n); // add n licenses to available
int removelicenses(int n); // decrement licenses by n
int destlicense(); // Destruct license
void logmsg(const char* msg); // log message to logfile

// Argument struct for semctl
union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

// Shared memory struct
struct license_mem {
    int nlicenses;
};

// public member attributes
extern struct license_mem* shared_mem;

#endif
