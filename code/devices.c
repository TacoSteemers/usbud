#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include "devices.h"
#include "global.h"

void initializeDeviceBookKeeping()
{
	int i = 0;
	device* newDevice = malloc(sizeof(device));
	memset(newDevice,0,sizeof(device));
	newDevice->id = malloc(sizeof(char));
	newDevice->id[0] = 0;
	for(; i < MAXNUMDEVS; i++)
	{
		memcpy(&gDevicesOnPreviousRun[i], newDevice, sizeof(device));
	}
	free(newDevice);
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
		

		syslog(LOG_DEBUG, "Device is already known.");
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

		
		memcpy(gDevicesOnPreviousRun[i], &d, sizeof(device));
		syslog(LOG_DEBUG, "Device \"%s\" has been registered.", d.id);
		return;
	}
	syslog(LOG_DEBUG, "Device \"%s\" could not be registered.", d.id);
}
