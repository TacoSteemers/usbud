#ifndef GLOBAL_H
#define GLOBAL_H

/* declared in devices.h */

#define MAXNUMDEVS 256*(sizeof(char *))
device* gDevicesOnPreviousRun[MAXNUMDEVS];
extern const char *gTargetDirectory; /* defined in processArguments.c */

#endif
