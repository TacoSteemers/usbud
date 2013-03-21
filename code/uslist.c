/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "detection.h"

const int gModeSetting = LISTMODE; /* Declared in global.h */
/* This program uses the 'list mode' of the code base. */

int gPrintDetailsInListMode = 0;

int main(int argc, char *argv[]) 
{
    if(argc == 2)
    {
        if(strcmp(argv[1], "-d") == 0)
            gPrintDetailsInListMode = 1;
    }
    
	doCheck();

	exit(EXIT_SUCCESS);
}

/* The following functions with empty body are here,
 because it allows us to use an unmodified 'detection.c' */
void initializeRun() {}
void finalizeRun() {}
void processDevice(char* deviceId) {}
