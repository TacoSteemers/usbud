/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#include <stdlib.h> /* malloc and such */
#include <stdio.h> /* util.h uses FILE */
#include <unistd.h> /* getpid, fork, execv */
#include <syslog.h>
#include <string.h>
#include "global.h"
#include "util.h"
#include "bookKeeping.h"
#include "backup.h"

void processDevice(char* mountPoint, char* deviceId)
{
	int index;
	if(checkIfDeviceIsKnown(deviceId)==1)
		return; /* Already known, and thus backed up */
	index = registerDevice(deviceId);
	if(index == -1)
		return; /* Failed to get the device registered */
	backDeviceUp(mountPoint, deviceId);
}

void backDeviceUp(char* mountPoint, char* deviceId)
{
	int blacklisted = 0;
	int whitelisted = 0;
	int needsWhitelisting = 0;
    if(gBlacklist[0] != NULL)
	{
		blacklisted = contains(gBlacklist, deviceId);
	}
    if(gWhitelist[0] != NULL)
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
	free(target);
}

void spawnBackup(char *source, char *target){
	int pid;	
	pid = getpid();
    
    /* Create copy of current process */
    pid = fork();

    /* The parent`s new pid will be 0 */
    if(pid != 0)
	{	
	    /* Do backup using rsync */
		/* In regards to the spaces, I have tried numerous 
			combinations of ', " and '\0', in all their 
			escaped glory, to no avail.
		   Even the usual trick for escaping both a local 
			and a remote shell does not work.
		   In the end I decided to just remove them. */
		char* targetNoSpaces = replace(target, " ", "");
		char *args2[] = {"rsync", "-Pr", source, targetNoSpaces, (char *) 0};
		syslog(LOG_INFO, "Device will be backed up in \"%s\"", targetNoSpaces);
		execvp("rsync", args2);	
		syslog(LOG_ERR, "Failed to backup: %s->%s", source, targetNoSpaces);
		free(targetNoSpaces);
		exit(EXIT_SUCCESS);
	}
}
