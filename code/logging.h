/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

/* Sloppy partial implementation of syslog functionality. 
   This allows the code that uses the syslog function to
    compile on a Windows platform. */

#ifndef DEVICE_H
#define DEVICE_H

    #ifdef _WIN32

        #include <stdio.h>
        int mode;
        FILE *logFile;
        void syslog(int mode, char *formatting, ...);
        void openlog(char* identifier, int unused_0, int unused_1);
        int setlogmask(int mode);
        int LOG_UPTO(int mode);
        #define LOG_EMERG   0
        #define LOG_ALERT   1
        #define LOG_CRIT    2
        #define LOG_ERR     3
        #define LOG_WARNING 4
        #define LOG_NOTICE  5
        #define LOG_INFO    6
        #define LOG_DEBUG   7

        /* The following are defined,
            but the corresponding 
            functionality does not exist. */
        #define LOG_CONS   0
        #define LOG_PID    0
        #define LOG_NDELAY 0
        #define LOG_LOCAL1 0
    #else

        #include <syslog.h>

    #endif
    
#endif