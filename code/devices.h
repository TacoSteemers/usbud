#ifndef DEVICE_H
#define DEVICE_H

typedef struct 
{
	char* id;
	int index;
	int runId;
} device;

void initializeDeviceBookKeeping(void);
void initializeRun(void);
void finalizeRun(void);
void processDevice(device d);
int  checkIfDeviceIsKnown(device d);
void registerDevice(device d);
void backDeviceUp(device d);
#endif
