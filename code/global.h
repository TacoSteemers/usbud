#ifndef GLOBAL_H
#define GLOBAL_H

#define MAXNUMDEVS 2
device* gDevicesOnPreviousRun[MAXNUMDEVS];
extern const char *gTargetDirectory; /* defined in processArguments.c */
extern int gCurrentRunId; /* defined in daemon.c */

#endif
