/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#include <stdlib.h> /* exit, malloc */
#include <syslog.h>
#include <string.h>
#include "bookKeeping.h"
#include "global.h"
#include "backup.h"

device* devices[MAXNUMDEVS];

void initializeDeviceBookKeeping(void)
{
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		devices[i] = malloc(sizeof(device));
		if(!devices[i])
		{
			syslog(LOG_ERR, "Exiting with failure: malloc failed during initialization");
			exit(EXIT_FAILURE);
		}
		devices[i]->index = i;
		devices[i]->runId = 0;
		devices[i]->id = malloc(sizeof(char) * 256);
		if(!devices[i]->id)
		{
			syslog(LOG_ERR, "Exiting with failure: malloc failed during initialization");
			exit(EXIT_FAILURE);
		}
		devices[i]->id[0] = '\0';
	}
}

void initializeRun(void)
{
	/* Ensure that the previous gCurrentRunId is different 
		from the current, and is not equal to zero. */
	gCurrentRunId += 1;
	if(gCurrentRunId > 2)
	{
		gCurrentRunId = 1;
	}
}

void finalizeRun(void)
{
	int i = 0;
	/* Here we will remove stale data from bookkeeping */
	for(; i < MAXNUMDEVS; i++)
	{
		if(devices[i]->id[0] == 0)
			continue; /* No data, no need to remove */
		if(devices[i]->runId == gCurrentRunId)
			continue; /* Recent data, no need to remove */
		syslog(LOG_INFO, "Removing stale device \"%s\".", devices[i]->id);
		devices[i]->id[0] = '\0';
		devices[i]->runId = 0;
	}
}

int checkIfDeviceIsKnown(char* deviceId)
{
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		if(strncmp(devices[i]->id, deviceId, strlen(deviceId)) != 0)
			continue; /* Not the same */
		/* This is the same. We will update the runId, 
			to show that this device is not stale */
		devices[i]->runId = gCurrentRunId; 
		return 1;
	}
	return 0;
}

int registerDevice(char* deviceId)
{
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		if(devices[i]->id[0] != '\0')
			continue; /* Not an empty spot */
		memcpy(devices[i]->id, deviceId, strlen(deviceId)+1);
		devices[i]->runId = gCurrentRunId;
		syslog(LOG_INFO, "Device \"%s\" has been registered.", deviceId);
		return i;
	}
	syslog(LOG_INFO, "Device \"%s\" could not be registered. Too many devices have been registered already.", deviceId);
	return -1;
}
