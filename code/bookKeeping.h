/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#ifndef DEVICE_H
#define DEVICE_H

typedef struct 
{
	char* id;
	int index;
	int runId;
} device;

#define MAXNUMDEVS 256
device* devices[MAXNUMDEVS];

void initializeDeviceBookKeeping(void);
void initializeRun(void);
void finalizeRun(void);
int  checkIfDeviceIsKnown(char* deviceId);
int registerDevice(char* deviceId);

#endif
