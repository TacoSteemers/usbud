// USB storage backup

// The general deamon code is adapted from
// http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
// Information on syslog can be found on
// http://www.linuxselfhelp.com/gnu/glibc/html_chapter/libc_18.html

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
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

    /* The Big Loop */
    while (1) {
	   doCheck();
       
       sleep(30); /* wait 30 seconds */
    }

   exit(EXIT_SUCCESS);
}
