/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#ifndef PROCESSARGUMENTS_H
#define PROCESSARGUMENTS_H

void processArguments(const char * const *arr, int arrc);
void initializeArgumentProcessing(void);
void setTarget(const char * const *arr, int arrc, int indexOfArgument);
int getArgumentSwitchInt(const char* argument);
void logArgumentReSetIgnored(const char* argument);
void loadListFromFile(char **out, const char * const *arr, int arrc, int indexOfArgument);

const char* signalTarget; /* defined in processArguments.c */
const char* signalBlacklist; /* defined in processArguments.c */
const char* signalWhitelist; /* defined in processArguments.c */

#endif
