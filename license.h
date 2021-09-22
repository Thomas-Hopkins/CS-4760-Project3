#ifndef __LICENSE_H
#define __LICENSE_H

// Public member functions
int getlicense(); // Block until license is available
int returnlicense(); // increment available licenses
int initlicense(); // initialize license object
int addtolicense(int n); // add n licenses to available
int removelicenses(int n); // decrement licenses by n
void logmsg(const char* msg); // log message to logfile

// public member attributes
char* logfile;
int nlicenses; 

#endif
