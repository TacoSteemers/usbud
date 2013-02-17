#include <stdlib.h> /* malloc and such */
#include <unistd.h> /* getpid, fork, execv */
#include <syslog.h>
#include <string.h>
#include "global.h"
#include "util.h"
#include "bookKeeping.h"
#include "backup.h"

void processDevice(char* mountPoint, char* deviceId)
{
	device *deviceContainer;
	if(checkIfDeviceIsKnown(deviceId)==1)
		return; /* Already known, and thus backed up */
	registerDevice(deviceContainer, deviceId);
	if(!deviceContainer)
		return; /* Failed to get the device registered */
	backDeviceUp(mountPoint, deviceId);
}

void backDeviceUp(char* mountPoint, char* deviceId)
{
	int blacklisted = 0;
	int whitelisted = 0;
	int needsWhitelisting = 0;
    if(gBlacklist != NULL)
	{
		blacklisted = contains(gBlacklist, deviceId);
	}
    if(gWhitelist != NULL)
	{
		needsWhitelisting = 1;
		whitelisted = contains(gWhitelist, deviceId);
	}
	if((blacklisted == 1) && (whitelisted == 1))
	{
		syslog(LOG_INFO, "Device \"%s\" has been blacklisted as well as whitelisted. It will not be backed up.", deviceId);
		return;
	}
	if(blacklisted == 1)
	{
		syslog(LOG_INFO, "Device \"%s\" has been blacklisted. It will not be backed up.", deviceId);
		return;
	}
	if(needsWhitelisting && !whitelisted)
	{
		syslog(LOG_INFO, "Device \"%s\" has not been whitelisted. It will not be backed up.", deviceId);
		return;
	}

	syslog(LOG_INFO, "Device \"%s\" will be backed up.", deviceId);

	/* Create target directory string */
	char* target = malloc(strlen(gTargetDirectory) + strlen(deviceId) + 2);
	if(!target)
	{
		syslog(LOG_ERR, "Exiting with failure: malloc failed during backDeviceUp");
		exit(EXIT_FAILURE);
	}
	memcpy(target, gTargetDirectory, strlen(gTargetDirectory)+1);
	target[strlen(gTargetDirectory)+1] = '\0';	
	strcat(target, "/");
	strcat(target, deviceId);

	spawnBackup(mountPoint, target);  /* Defined in backup.c */
}

void spawnBackup(char *source, char *target){
	int pid;	
	pid = getpid();
    
    /* Create copy of current process */
    pid = fork();

    /* The parent`s new pid will be 0 */
    if(pid != 0)
	{	
	    pid = fork();
		if(pid != 0)
		{	/* Create target directory */
			char *args1[] = {"mkdir", target, (char *) 0};
			execvp("mkdir", args1);
			syslog(LOG_ERR, "Failed to create directory: %s \n", target);
		}
		else 
		{	/* Do backup using rsync */
			/* Escape target directory name with quotes 
				(because of spaces) */
			char* targetWithQuotes = malloc(strlen(target)+3);
			targetWithQuotes[0] = '\"';
			memcpy(targetWithQuotes+1, target, strlen(target)+1);
			strcat(targetWithQuotes, "\"");
			char *args2[] = {"rsync", "-Pr", source, targetWithQuotes, (char *) 0};
			execvp("rsync", args2);	
			syslog(LOG_ERR, "Failed to backup: %s->%s", source, targetWithQuotes);
		}
		exit(EXIT_SUCCESS);
	}
}
