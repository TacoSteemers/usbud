#include <stdlib.h>
#include "global.h"
#include "detection.h"

const int gModeSetting = LISTMODE; /* Declared in global.h */
/* This program uses the 'list mode' of the code base. */

int main(int argc, char *argv[]) 
{
	doCheck();

	exit(EXIT_SUCCESS);
}

/* The following functions with empty body are here,
 because it allows us to use an unmodified 'detection.c' */
void initializeRun() {}
void finalizeRun() {}
void processDevice(char* deviceId) {}
