/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#include <stdlib.h> /* exit, malloc and such */
#include <unistd.h> /* getpid, fork, execv */
#include <string.h>
#include <errno.h>
#include <stdio.h> /* util.h uses FILE */
#include <dirent.h> /* util.h uses DIR */
#include "logging.h"
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

	doBackup(mountPoint, target);  /* Defined in backup.c */
	free(target);
}

void doBackup(char *source, char *target){
    char errorBuf[32];
    size_t errorBufLen = 32;
    int error;
    char outputBuffer[32];
    char command[8192]; /* Max path length * 2 */
    char* targetNoSpaces = replace(target, " ", "");
    tidyStringUp(targetNoSpaces);
    sprintf(command, "%s %s %s %s", "rsync", "-Prtu", source, targetNoSpaces);
    syslog(LOG_INFO, "Backup command: %s", command);
    FILE *outputPipe = popen (command, "r");
    if (!outputPipe)
    {
        error = errno;
        strerror_r(error, errorBuf, errorBufLen);
        syslog(LOG_ERR, "Execution of backup failed: %s", errorBuf);
        syslog(LOG_ERR, "Exiting with failure: could not run command: \"%s\"", command);
        exit(EXIT_FAILURE);
    }
    while(!feof(outputPipe)) {
        fgets(outputBuffer, sizeof(outputBuffer), outputPipe);
        /* Logging of backup can be implemented here */
    }
    if (pclose(outputPipe) == -1)
    {
        error = errno;
        strerror_r(error, errorBuf, errorBufLen);
        syslog(LOG_ERR, "Error while closing the pipe to the backup failed: %s", errorBuf);
    }
    if(targetNoSpaces != NULL)
        free(targetNoSpaces);
    syslog(LOG_INFO, "Execution of backup done.");
}
