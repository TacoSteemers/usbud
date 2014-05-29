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
#include <syslog.h>
#include "global.h"
#include "util.h"
#include "bookKeeping.h"
#include "backup.h"

void perhapsPerformBackup(char* mountPoint, char* deviceId)
{
    int index;
    if(checkIfDeviceIsKnown(deviceId)==1)
        return; /* Already known, and thus backed up */
    index = registerDevice(deviceId);
    if(index == -1)
        return; /* Failed to get the device registered */
    if(!needsBackup(deviceId))
        return;
    /* Device needs backing up! */

    char* target = 
        malloc(strlen(gTargetDirectory) + strlen(deviceId) + 2);
    if(!target)
    {
        syslog(LOG_ERR, 
            "Exiting with failure: malloc failed during backDeviceUp");
        exit(EXIT_FAILURE);
    }
    createTargetDirectoryString(target, deviceId);
    doBackup(mountPoint, target);
    free(target);
}

int needsBackup(char* deviceId)
{
    int blacklisted = 0;
    int whitelisted = 0;
    int needsWhitelisting = 0;
    if(gBlacklist[0] != NULL)
    {
        blacklisted = contains((const char * const *)gBlacklist, 
            deviceId);
    }
    if(gWhitelist[0] != NULL)
    {
        needsWhitelisting = 1;
        whitelisted = contains((const char * const *)gWhitelist, 
            deviceId);
    }
    if((blacklisted == 1) && (whitelisted == 1))
    {
        syslog(LOG_INFO, "Device \"%s\" has been blacklisted as %s",
            deviceId, "well as whitelisted. It will not be backed up.");
        return 0;
    }
    if(blacklisted == 1)
    {
        syslog(LOG_INFO, "Device \"%s\" has been blacklisted. %s", 
            deviceId,
            "It will not be backed up.");
        return 0;
    }
    if(needsWhitelisting && !whitelisted)
    {
        syslog(LOG_INFO, "Device \"%s\" has not been whitelisted. %s", 
            deviceId,
            "It will not be backed up.");
        return 0;
    }
    syslog(LOG_INFO, "Device \"%s\" will be backed up.", deviceId);
    return 1;
}

void createTargetDirectoryString(char* target, char* deviceId)
{
    memcpy(target, gTargetDirectory, strlen(gTargetDirectory)+1);
    target[strlen(gTargetDirectory)+1] = '\0';    
    strcat(target, "/");
    strcat(target, deviceId);
}

void doBackup(char *source, char *target)
{
    char errorBuf[32];
    size_t errorBufLen = 32;
    char outputBuffer[32];
    char command[8192]; /* Max path length * 2 */
    char notifyBefore[1024]; 
    char notifyAfter[1024]; 
    char* targetNoSpaces = replace(target, " ", "");
    tidyStringUp(targetNoSpaces);
    if(gNotificationSetting != NONOTIFICATIONS)
    {
        char notifyBaseBefore[512];
        char notifyBaseAfter[512];
        sprintf(notifyBaseBefore, "Starting backup of %s.", source);
        sprintf(notifyBaseAfter, "Finished backup of %s.", source);
        
        if(gNotificationSetting == NOTIFYSENDMODE)
        {
            sprintf(notifyBefore, 
                "notify-send -i stock_save-as \"USB Storage Backup\" \"%s\"", 
                notifyBaseBefore);
            sprintf(notifyAfter,  
                "notify-send -i stock_save-as \"USB Storage Backup\" \"%s\"", 
                notifyBaseAfter);
        }
        if(gNotificationSetting == XMESSAGEMODE)
        {
            sprintf(notifyBefore, "xmessage \"USB Storage Backup %s\" -timeout 3", 
                notifyBaseBefore);
            sprintf(notifyAfter,  "xmessage \"USB Storage Backup %s\" -timeout 3", 
            notifyBaseAfter);
        }        
        sprintf(command, "%s; %s %s %s %s; %s", notifyBefore, 
            "rsync", "-Prtu", source, targetNoSpaces, notifyAfter);
    }
    else
    {
        sprintf(command, "%s %s %s %s", "rsync", "-Prtu", source, 
            targetNoSpaces);
    }    
    syslog(LOG_INFO, "Backup command: %s", command);
    FILE *outputPipe = popen (command, "r");
    if (!outputPipe)
    {
        strerror_r(errno, errorBuf, errorBufLen);
        syslog(LOG_ERR, "Execution of backup failed: %s", errorBuf);
        syslog(LOG_ERR, 
            "Exiting with failure: could not run command: \"%s\"", 
            command);
        exit(EXIT_FAILURE);
    }
    while(!feof(outputPipe)) {
        fgets(outputBuffer, sizeof(outputBuffer), outputPipe);
    }
    if (pclose(outputPipe) == -1)
    {
        strerror_r(errno, errorBuf, errorBufLen);
        syslog(LOG_ERR, 
            "Error. Closing the pipe to the backup process failed: %s", 
            errorBuf);
    }
    syslog(LOG_INFO, "Execution of backup done.");
    free(targetNoSpaces);
}
