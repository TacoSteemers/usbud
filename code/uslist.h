/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#ifndef USLIST_H
#define USLIST_H

void initializeRun(void);
void finalizeRun(void);
void processDevice(char* deviceId);
#endif
