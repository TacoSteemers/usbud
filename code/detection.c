/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

/* Many thanks to Orwellophile and his answer on 
    http://stackoverflow.com/questions/2432759/ */

#include <dirent.h>
#include <unistd.h> /* read, readlink */
#include <string.h>
#include <stdio.h> /* fopen, feof, fgets, FILE */
#include <stdlib.h> /* exit */
#include <fcntl.h> /* File control options (contains open) */
#include <syslog.h>
#include "detection.h"
#include "global.h"
#include "bookKeeping.h"
#include "backup.h"
#include "util.h"

void doCheck(void) 
{
    DIR *directory;
    struct dirent *entry;
    char* dirToCheck = "/sys/block/";
    char devicePath[MAXDEVICEPATHLENGTH];
    char* partitions[MAXNUMPARTITIONS];
    int count;
    int *pCount = &count;

    initializeRun();

    directory = openDirOrHang(dirToCheck);
    /* Proccess each partition related to a device name in /sys/block/, 
        that starts with "sd"*/
    do 
    {
        if ((entry = readdir(directory)) == NULL) 
            continue;
        if(entry->d_name[0]!='s' || entry->d_name[1]!='d')
            continue; 
        /* sda, sdb, sdc, ... */

        sprintf(devicePath, "/dev/%s", entry->d_name);
        count = 0;
        findMountedPartitions(partitions, pCount, devicePath);
        
        int i;
        for(i = 0; i < count; i++)
        {
            processPartition(devicePath, entry->d_name, partitions[i]);
            free(partitions[i]);
        }
    } while (entry != NULL);
    free(directory);
    free(entry);

    finalizeRun();
}

void processPartition(char *devicePath, 
    char *deviceName, char *partition) 
{
    /* Examples of arguments:
        processItem("/dev/sdh", "sdh", "/dev/sdh1");
        processItem("/dev/sdh", "sdh", "/dev/sdh2");
     */ 
     
    ssize_t len; /* Length of the path to the symbolic link */
    char buf[MAXIDLENGTH];
    /* strLocation will contain the symbolic link in /sys/block/,
        that leads to the device that we want to investigate */
    char strLocation[MAXIDLENGTH];
    /* strDevice will contain the path to the device location in 
        /sys/devices */
    char strDevice[MAXIDLENGTH];
    /* strId will contain the identifier for the device, that will be 
        used by this code base */
    char strId[MAXIDLENGTH];
    /* mountPoint will contain the path to which the partition
        has been mounted */
    char mountPoint[MAXMOUNTPOINTLENGTH];
    getMountPoint(mountPoint, partition);

    /* Next we get the relative path to the device location in 
        /sys/devices, from the symbolic link in /sys/block */
    snprintf(strLocation, sizeof strLocation, 
        "/sys/block/%s", deviceName);    
    len = readlink(strLocation, buf, MAXIDLENGTH);
    buf[len] = 0;
    sprintf(strDevice, "%s/%s", "/sys/block/", buf);
    
    /* Now we try to gather device information 
        such as manufacturer, product and serial number.
       strDevice will be changed in the process. 
       We use a variable to make note of the length,
        to be sure that we can properly zero-terminate. */
    int intIdLen = 0;
    getDeviceInfo(strId, &intIdLen, strDevice);
    
    if(intIdLen==0)
    {
        /* Some kind of device that is not of interest
           Most likely it doesn't have a 'serial' file, in which case
            it isn't a normal USB storage device
        */
        return; 
    }
        
    /* strId will end with the number of the partition */
    strcat(strId, partition + strlen(devicePath));

    if(gModeSetting == LISTMODE)
    {    /* In list mode, we are only interested in 
             listing the devices */
        if(gPrintDetailsInListMode == 1)
        {
            printf("%s, %s, %s\n", devicePath, mountPoint, strId);            
        }
        else
        {
            printf("%s\n", strId);            
        }
        return;
    }
    
    /* It appears we have a valid device, we should 
        back it up if we are configured to do so */
    perhapsPerformBackup(mountPoint, strId); /* Defined in backup.c */
}

void getDeviceInfo(char* out, int *pOutLen, const char device[])
{
    int i;
    char *p;
    /* strModel will contain the identifier for the model of this device, 
        as reported by the device. 
      This is of interest in the case of multi-card readers. */
    char strModel[MAXIDLENGTH];
    int intModelLen = 0;

    /* strrchr: Locate last occurrence of character in string
                Returns a pointer to the last occurrence of 
                 character in the C string str.
       We will be using that function to deconstruct the device
        variable into the parts that interest us.
       We do this to get the proper device location (which 
        USB connector our device is plugged in to), and the
        subsystem number. That number we use to get the device
        model. 
       In the case of a simple thumb drive, the subsystem number 
        will be 0. 
       A card reader with multiple slots will have multiple 
        subsystems, each with it's own number.
       If we are looking at a SATA device instead of a USB device,
        then this is where things will start to look different 
        than expected. */

    /* Retrieving subsystem (ends in subsystem id) */
    for (i=0; i<2; i++) {
        p = strrchr(device, '/');
        *p = 0;
    }
    /* Retrieving model name from subsystem */
    addDeviceInfo(strModel, &intModelLen, device, "/model");

    /* Retrieving the general system information */
    for (i=0; i<4; i++) {
        p = strrchr(device, '/');
        *p = 0;
    }
    addDeviceInfo(out, pOutLen, device, "/manufacturer");
    if (pOutLen > 0)
        out[*pOutLen-1] = ' ';
    addDeviceInfo(out, pOutLen, device, "/product");
    if (pOutLen > 0)
        out[*pOutLen-1] = ' ';
    addDeviceInfo(out, pOutLen, device, "/serial");
    if (pOutLen <= 0)
        return; /* Apparently this is not a storage device 
                    that is compatible with this daemon.
                    An example of that is a SATA device. */

    tidyStringUp(out);
    tidyStringUp(strModel);

    /* Process device model name, if any */
    if(strlen(strModel) != 0)
    {
        if((strlen(strModel) + strlen(out) + 2) >= MAXIDLENGTH)
        {
            /* There is not enough space to add a space and 
                the model to the id, so we create that space. */
            out[MAXIDLENGTH - strlen(strModel) - 2] = 0;
        }
        strcat(out, " ");
        strcat(out, strModel);
    }
}

void addDeviceInfo(char *out, int *pOutLen, const char device[], const char property[])
{
    char source[MAXIDLENGTH];
    char buf[MAXIDLENGTH];

    /* Make sure that out has a null character to terminate it, 
        this is necessary when using strcat */
    out[*pOutLen] = 0;

    /* Constructing source of desired info */
    strcpy(source, device);
    strcat(source, property);

    /* Concatenating desired info to target,
       if such info appears to be found */
    int f = open(source, 0);
    if (f == -1)
        return;
    int len = read(f, buf, MAXIDLENGTH);
    if (len <= 0)
        return;

    /* Update the out variables */
    *pOutLen += len;
    buf[len-1] = 0;
    strcat(out, (const char*) buf);
    tidyStringUp(out);
    *pOutLen = strlen(out) + 1;
}

char* findMountedPartitions(char* output[], 
    int* pCount, char *devicePath)
{
    char buf[MAXDEVICEPATHLENGTH];
    FILE *file = openOrHang("/proc/mounts", "r"); /* util.c */
    while(!feof(file)) {
        if (fgets(buf,sizeof(buf),file)) {
            /* Check if this line starts with our item */
            if(strncmp(buf, devicePath, strlen(devicePath)) != 0)
                continue;
            /* It does */
            char* p = strchr(buf, ' ');
            //p = p +1;
            *p = 0;
            output[*pCount] = malloc(MAXDEVICEPATHLENGTH);
            memcpy(output[*pCount], buf, strlen(buf)+1);
            *pCount = *pCount + 1;
        }
    }
    fclose(file);
    return 0;
}

void getMountPoint(char *out, char *devicePath)
{
    char buf[MAXMOUNTPOINTLENGTH];
    char *p1; /* To point at the start of the mount point */
    char *p2; /* To point at the end of the mount point */
    FILE *file = openOrHang("/proc/mounts", "r"); /* util.c */
    out[0] = '\0'; /* Necessary for strcat */
    while(!feof(file)) {
        if (fgets(buf,sizeof(buf),file)) {
            /* Check if this line starts with our item */
            if(strncmp(buf, devicePath, strlen(devicePath)) != 0)
                continue;
            /* It does */
            p1 = strchr(buf, ' '); 
            p1 = p1 + 1;
            p2 = strchr(p1, ' ');
            *p2 = 0;
            strcat(out, p1);
            break;
        }
    }
    fclose(file);
}
