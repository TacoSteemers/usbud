#include <stdlib.h>
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
		syslog(LOG_DEBUG, "Removing stale device \"%s\" with runId %d, at index %d (%d).", 
			devices[i]->id, 
			devices[i]->runId, 
			devices[i]->index, 
			i);
		devices[i]->id[0] = '\0';
		devices[i]->runId = 0;
		syslog(LOG_DEBUG, "Removed stale device.");
	}
}

int checkIfDeviceIsKnown(char* deviceId)
{
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		if(strncmp(devices[i]->id, deviceId, strlen(deviceId)) != 0)
			continue; /* Not the same */
		devices[i]->runId = gCurrentRunId;
		syslog(LOG_DEBUG, "Device is already known (as \"%s\", at index %d (%d)). runId has been updated.", 
			devices[i]->id, 
			devices[i]->index, 
			i);
		return 1;
	}
	return 0;
}

void registerDevice(device *out, char* deviceId)
{
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		if(devices[i]->id[0] != '\0')
			continue; /* Not an empty spot */
		memcpy(devices[i]->id, deviceId, strlen(deviceId)+1);
		devices[i]->runId = gCurrentRunId;
		syslog(LOG_DEBUG, "Device \"%s\" has been registered (as \"%s\", at index %d (%d)).", 
			deviceId, 
			devices[i]->id,
			devices[i]->index, 
			i);
		out = devices[i];
		return;
	}
	syslog(LOG_DEBUG, "Device \"%s\" could not be registered. Too many devices have been registered already.", deviceId);
}
