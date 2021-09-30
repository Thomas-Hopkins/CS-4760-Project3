#ifndef __LICENSE_H
#define __LICENSE_H

#include "config.h"

// Public member functions
int attachsharedmem(); // Attach to shared memory
int getlicense(); // Block until license is available
int returnlicense(); // increment available licenses
int initlicense(); // initialize license object
int addtolicense(int n); // add n licenses to available
int removelicenses(int n); // decrement licenses by n
int destlicense(); // Destruct license
void logmsg(const char* msg); // log message to logfile

// Shared memory struct
struct license_mem {
    int nlicenses;
    int choosing[MAX_PROCESSES];
    int number[MAX_PROCESSES];
};

// public member attributes
extern struct license_mem* shared_mem;

#endif
