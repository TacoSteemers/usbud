/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#include <sys/stat.h> /* umask */
#include <stdlib.h> /* exit */
#include <unistd.h> /* sleep */
#include <string.h>
#include "logging.h"
#include "processArguments.h"
#include "detection.h"
#include "bookKeeping.h"
#include "global.h"
#ifdef _WIN32
    #include "windowsAlternatives.h"
	#include <process.h> /* Required for _spawnv */
	#include <windows.h>
	/* We make getpid() work in a similar 
		way on Windows as it does on Linux */
	#define getpid() GetCurrentProcessId()
#endif

int gCurrentRunId = 0;      /* Declared in global.h */
const int gModeSetting = DAEMONMODE; /* Declared in global.h */
/* This program uses the 'daemon mode' of the code base. */

int main(int argc, char *argv[]) {
    /* Our process ID and Session ID */
    pid_t pid;
    
    printf("Original pid: %ld", getpid());
    
    
    /* Fork off the parent process */
    _spawnv(_P_NOWAIT, argv[0], (const char * const *)argv);
    
    pid = getpid();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Open any logs here */   
	setlogmask (LOG_UPTO (LOG_DEBUG));  
	openlog ("usbstoragebackup", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(LOG_NOTICE, "Program started by User %d", getuid ());
            
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
       
       Sleep(30000); /* wait 30 seconds */
    }

   exit(EXIT_SUCCESS);
}
