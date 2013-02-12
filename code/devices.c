#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include "devices.h"
#include "global.h"

void initializeDeviceBookKeeping(void)
{
	int i = 0;
	device* newDevice = malloc(sizeof(device));
	memset(newDevice,0,sizeof(device));
	newDevice->id = malloc(sizeof(char));
	newDevice->id[0] = 0;
	newDevice->runId = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		newDevice->index = i;
		memcpy(&gDevicesOnPreviousRun[i], newDevice, sizeof(device));
	}
	free(newDevice);
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
		if(gDevicesOnPreviousRun[i]->id[0] == 0)
			continue; /* No data, no need to remove */
		if(gDevicesOnPreviousRun[i]->runId == gCurrentRunId)
			continue; /* Recent data, no need to remove */
		syslog(LOG_DEBUG, "Removing stale device \"%s\" with runId %d, at index %d (%d).", 
			gDevicesOnPreviousRun[i]->id, 
			gDevicesOnPreviousRun[i]->runId, 
			gDevicesOnPreviousRun[i]->index, 
			i);
		gDevicesOnPreviousRun[i]->id[0] = 0;
		gDevicesOnPreviousRun[i]->runId = 0;
		syslog(LOG_DEBUG, "Removed stale device.");
	}
}

void processDevice(device d)
{
	if(checkIfDeviceIsKnown(d)==1)
		return; /* Already known, and thus backed up */
	registerDevice(d);
	backDeviceUp(d);
}

int checkIfDeviceIsKnown(device d)
{
	syslog(LOG_DEBUG, "checkIfDeviceIsKnown \"%s\".", d.id);
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		if(gDevicesOnPreviousRun[i]->id[0] == 0)
			continue; /* Empty spot */
		if(gDevicesOnPreviousRun[i]->id!=d.id)
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

void registerDevice(device d)
{
	syslog(LOG_DEBUG, "registerDevice \"%s\".", d.id);
	int i = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		if(gDevicesOnPreviousRun[i]->id[0] != 0)
			continue; /* Not an empty spot */
		d.runId = gCurrentRunId;
		d.index = i;
		memcpy(gDevicesOnPreviousRun[i], &d, sizeof(device));
		syslog(LOG_DEBUG, "Device \"%s\" has been registered (as \"%s\").", d.id, gDevicesOnPreviousRun[i]->id);
		return;
	}
	syslog(LOG_DEBUG, "Device \"%s\" could not be registered.", d.id);
}

void backDeviceUp(device d)
{
	syslog(LOG_DEBUG, "backDeviceUp \"%s\". Not yet implemented.", d.id);
}
