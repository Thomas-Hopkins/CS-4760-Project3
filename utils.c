#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void outputerror(char* exe, char* msg, int EXIT_CODE) {
    fprintf(stderr, "%s: ", exe);
    perror(msg);
    if (EXIT_CODE != -1) exit(EXIT_CODE);
}