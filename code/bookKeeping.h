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
void processDevice(char* deviceId);
int  checkIfDeviceIsKnown(char* deviceId);
void registerDevice(char* deviceId);
void backDeviceUp(char* deviceId);
#endif
