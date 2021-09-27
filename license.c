
#include "license.h"
#include "config.h"
#include "lib/log.h"

int nlicenses = 0;

int getlicense() {
	while (nlicenses <= 0); // block
	
	return ++nlicenses;
}

int returnlicense() {
	nlicenses++;
	return 1;
}

int initlicense() {
	nlicenses = MAX_PROCESSES;
	return 0;	
}

int addtolicense(int n) {
	nlicenses += n;
	return 0;
}

int removelicenses(int n) {
	nlicenses -= n;
	return 0;
}

void logmsg(const char* msg) {
	addmsg(non_type, msg);
	savelog(LOG_FILE);
	clearlog();
}

