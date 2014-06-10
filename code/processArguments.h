/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#ifndef PROCESSARGUMENTS_H
#define PROCESSARGUMENTS_H

void processArguments(int argc, char * const *argv);
void initializeArgumentProcessing(void);
void logArgumentReSetIgnored(const char* argument);
void setTarget(char* optarg);
void loadListFromFile(char **out, char* optarg);
void setNotificationMode(char* optarg);

const char* signalTarget; /* defined in processArguments.c */
const char* signalBlacklist; /* defined in processArguments.c */
const char* signalWhitelist; /* defined in processArguments.c */

/* For determining which way the user wishes to be notified */
const char* signalNotificationSetting; /* defined in processArguments.c */
const char* signalNotifySend; /* defined in processArguments.c */
const char* signalXMessage; /* defined in processArguments.c */
#endif

