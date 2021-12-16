/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#ifndef GLOBAL_H
#define GLOBAL_H

/* Constants used for finding partitions and parsing lists */
#define MAXLISTLENGTH 256
#define MAXIDLENGTH 256
/* MAXDEVICEPATHLENGTH is 260 because of 255 plus 5 for /dev/ and 1 for the overhead. */
#define MAXDEVICEPATHLENGTH 261
#define MAXMOUNTPOINTLENGTH 1024
#define MAXNUMPARTITIONS 256 /* GPT primary partition limit * 2 */

/* A command-line argument for the uslist program */
int gPrintDetailsInListMode; /* defined in uslist.c */

/* Where the backups will end up */
const char *gTargetDirectory; /* defined in processArguments.c */

/* For becoming a daemon */
int gCurrentRunId; /* defined in daemon.c */

/* The mode seting is used to determine whether we are using uslist or usbud
   functionality. */
const int gModeSetting; /* Defined in either daemon.c or uslist.c */
#define DAEMONMODE 1
#define LISTMODE 2

/* The blacklist and the white list used by usbud. 
   Each used slot contains a device identifier, as determined by uslist.
   The daemon expects the used slots to be at the start of the list, 
    without gaps.
   Also expects each unused item to be null or start with a '\0'.
   The daemon will not continue any comparisons after encountering
    such an item. */
char **gBlacklist; /* Defined in processArguments.c*/
char **gWhitelist; /* Defined in processArguments.c*/

/* Which type of notification should be used */
int gNotificationSetting; 
#define NONOTIFICATIONS 0 /* 0 is default for gNotificationSetting */
#define NOTIFYSENDMODE 1
#define XMESSAGEMODE 2

/* The interval between each scan */
long gIntervalSetting; /* defined in processArguments.c */
#define INTERVAL 30

#endif
