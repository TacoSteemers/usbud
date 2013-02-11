#ifndef DEVICE_H
#define DEVICE_H

typedef struct 
{
	char* id;
} device;

void initializeDeviceBookKeeping();
int checkIfDeviceIsKnown(device d);
void registerDevice(device d);
#endif
