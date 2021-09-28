#ifndef __LICENSE_H
#define __LICENSE_H

// Public member functions
int attachsharedmem(); // Attach to shared memory
int getlicense(); // Block until license is available
int returnlicense(); // increment available licenses
int initlicense(); // initialize license object
int addtolicense(int n); // add n licenses to available
int removelicenses(int n); // decrement licenses by n
int destlicense(); // Destruct license
void logmsg(const char* msg); // log message to logfile

// public member attributes
extern int* nlicenses; 

#endif
