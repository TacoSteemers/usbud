/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

/* The general deamon code is adapted from
   http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
   At the time of retrieval, the HOWTO itself used 'a BSD License', but one was 
	offered. As such I took it to use the 'Simplified BSD License', a copy of 
	which should have been provided with this program.

   Information on syslog can be found on
   http://www.linuxselfhelp.com/gnu/glibc/html_chapter/libc_18.html           */

#include <sys/stat.h> /* umask */
#include <stdlib.h> /* exit */
#include <unistd.h> /* sleep */
#include <syslog.h>
#include <string.h>
#include "processArguments.h"
#include "detection.h"
#include "bookKeeping.h"
#include "global.h"

int gCurrentRunId = 0;      /* Declared in global.h */
const int gModeSetting = DAEMONMODE; /* Declared in global.h */
/* This program uses the 'daemon mode' of the code base. */

int main(int argc, char *argv[]) {
    /* Our process ID and Session ID */
    pid_t pid, sid;
    
    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);
            
    /* Open any logs here */   
	setlogmask (LOG_UPTO (LOG_DEBUG));  
	openlog ("usbstoragebackup", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(LOG_NOTICE, "Program started by User %d", getuid ());
            
    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
		syslog(LOG_ERR, "Exiting with failure: Program did not get proper sid");
        exit(EXIT_FAILURE);
    }
    
    /* Change the current working directory */
    if ((chdir("/")) < 0) {
		syslog(LOG_ERR, "Exiting with failure: Program was unable to chdir");
        exit(EXIT_FAILURE);
    }
    
    /* Close out the standard file descriptors 
	   We won't need these, as such it is safer to close them */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    /* Initialization of the daemon functionality */
	initializeDeviceBookKeeping();
   	processArguments(argv, argc);
	if(gTargetDirectory == NULL)
	{
		syslog(LOG_ERR, "Exiting with failure: No target directory has been specified. Specifying a target directory is mandatory.");
        exit(EXIT_FAILURE);
	}

    /* The Big Loop */
    while (1) {
	   doCheck();
       
       sleep(30); /* wait 30 seconds */
    }

   exit(EXIT_SUCCESS);
}
