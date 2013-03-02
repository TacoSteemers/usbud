/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "logging.h"

/* Sloppy partial implementation of syslog functionality. 
   This allows the code that uses the syslog function to
    compile on a Windows platform. */

int mode = 0;
FILE *logFile;

void syslog(int requestedMode, char *formatting, ...)
{
    if(requestedMode > mode)
        return;
    va_list args;
    va_start(args,formatting);
    vfprintf(logFile, formatting,args);
    va_end(args);                     
}

int setlogmask(int newMode)
{
    int oldMode = mode;
    mode = newmode;
    return oldMode;
}

int LOG_UPTO(int newMode)
{
    return newMode;
}

void openlog(char* identifier, int unused_0, int unused_1)
{
    char[] filePath = char[4096];
    filePath[0] = '\0';
    strcat(filePath, getenv("TEMP"));
    strcat(filePath, "\\");
    strcat(filePath, identifier);
    logFile = fopen(filePath, "r");
}