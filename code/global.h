/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#ifndef GLOBAL_H
#define GLOBAL_H

const char *gTargetDirectory; /* defined in processArguments.c */
int gCurrentRunId; /* defined in daemon.c */

const int gModeSetting; /* Defined in either daemon.c or uslist.c */
#define DAEMONMODE 1
#define LISTMODE 2

#define MAXLISTLENGTH 256
#define MAXIDLENGTH 256
#define MAXDEVICEPATHLENGTH 32
#define MAXMOUNTPOINTLENGTH 1024
char **gBlacklist; /* Defined in processArguments.c*/
char **gWhitelist; /* Defined in processArguments.c*/
#endif
