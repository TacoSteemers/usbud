#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include "bookKeeping.h"
#include "global.h"

device* gDevicesOnPreviousRun[MAXNUMDEVS];

void initializeDeviceBookKeeping(void)
{
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		gDevicesOnPreviousRun[i] = malloc(sizeof(device));
		if(!gDevicesOnPreviousRun[i])
		{
			syslog(LOG_ERR, "Exiting with failure: malloc failed during initialization");
			exit(EXIT_FAILURE);
		}
		gDevicesOnPreviousRun[i]->index = i;
		gDevicesOnPreviousRun[i]->runId = 0;
		gDevicesOnPreviousRun[i]->id = malloc(sizeof(char) * 256);
		if(!gDevicesOnPreviousRun[i]->id)
		{
			syslog(LOG_ERR, "Exiting with failure: malloc failed during initialization");
			exit(EXIT_FAILURE);
		}
		gDevicesOnPreviousRun[i]->id[0] = '\0';
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
	syslog(LOG_DEBUG, "initializeRun (leaving) gCurrentRunId is %d.", gCurrentRunId);
}

void finalizeRun(void)
{
	syslog(LOG_DEBUG, "finalizeRun, gCurrentRunId is %d.", gCurrentRunId);
	int i = 0;
	/* Here we will remove stale data from bookkeeping */
	for(; i < MAXNUMDEVS; i++)
	{
		syslog(LOG_DEBUG, "Found device \"%s\" with runId %d, at index %d (%d).",
			gDevicesOnPreviousRun[i]->id,
			gDevicesOnPreviousRun[i]->runId,
			gDevicesOnPreviousRun[i]->index, 
			i);
		if(gDevicesOnPreviousRun[i]->id[0] == 0)
			continue; /* No data, no need to remove */
		if(gDevicesOnPreviousRun[i]->runId == gCurrentRunId)
			continue; /* Recent data, no need to remove */
		syslog(LOG_DEBUG, "Removing stale device \"%s\" with runId %d, at index %d (%d).", 
			gDevicesOnPreviousRun[i]->id, 
			gDevicesOnPreviousRun[i]->runId, 
			gDevicesOnPreviousRun[i]->index, 
			i);
		gDevicesOnPreviousRun[i]->id[0] = '\0';
		gDevicesOnPreviousRun[i]->runId = 0;
		syslog(LOG_DEBUG, "Removed stale device.");
	}
}

void processDevice(char* deviceId)
{
	if(checkIfDeviceIsKnown(deviceId)==1)
		return; /* Already known, and thus backed up */
	registerDevice(deviceId);
	backDeviceUp(deviceId);
}

int checkIfDeviceIsKnown(char* deviceId)
{
	syslog(LOG_DEBUG, "checkIfDeviceIsKnown \"%s\".", deviceId);
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		if(strncmp(gDevicesOnPreviousRun[i]->id, deviceId, strlen(deviceId)) != 0)
			continue; /* Not the same */
		gDevicesOnPreviousRun[i]->runId = gCurrentRunId;
		syslog(LOG_DEBUG, "Device is already known (as \"%s\", at index %d (%d)). runId has been updated.", 
			gDevicesOnPreviousRun[i]->id, 
			gDevicesOnPreviousRun[i]->index, 
			i);
		return 1;
	}

	syslog(LOG_DEBUG, "Device is not known.");
	return 0;
}

void registerDevice(char* deviceId)
{
	syslog(LOG_DEBUG, "registerDevice \"%s\".", deviceId);
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		if(gDevicesOnPreviousRun[i]->id[0] != '\0')
			continue; /* Not an empty spot */
		memcpy(gDevicesOnPreviousRun[i]->id, deviceId, strlen(deviceId)+1);
		gDevicesOnPreviousRun[i]->runId = gCurrentRunId;
		syslog(LOG_DEBUG, "Device \"%s\" has been registered (as \"%s\", at index %d (%d)).", 
			deviceId, 
			gDevicesOnPreviousRun[i]->id,
			gDevicesOnPreviousRun[i]->index, 
			i);
		return;
	}
	syslog(LOG_DEBUG, "Device \"%s\" could not be registered.", deviceId);
}

void backDeviceUp(char* deviceId)
{
	syslog(LOG_DEBUG, "backDeviceUp \"%s\". Not yet implemented.", deviceId);
}
