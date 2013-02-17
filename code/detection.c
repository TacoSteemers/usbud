// adapted from Orwellophile's answer on http://stackoverflow.com/questions/2432759/usb-drive-serial-number-under-linux-c

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h> // Contains system(), among other
#include <syslog.h>
#include <fcntl.h> // File control options (contains open())
#include "detection.h"
#include "bookKeeping.h"
#include "global.h"
#include "util.h"

void doCheck(void) 
{
    DIR *directory;
    struct dirent *entry;
    char* dirToCheck = "/sys/block/";

	initializeRun();

	if ((directory = opendir(dirToCheck)) == NULL) 
    {
		syslog(LOG_ERR, "couldn't open '%s'",  dirToCheck);  
        return;
    }
    do 
    {	/* Proccess each item in /sys/block/ */
        if ((entry = readdir(directory)) == NULL) 
        	continue;
		if(entry->d_name[0]!='s' || entry->d_name[1]!='d')
        	continue; /* sda, sdb, sdc, ... */
        processItem(entry->d_name);
    } while (entry != NULL);
	free(directory);

	finalizeRun();
}

void processItem(char *strInputPath) 
{
    ssize_t len; /* Length of the path to the symbolic link */
    char buf[256];
	/* strLocation will contain the symbolic link in /sys/block/,
		that leads to the device that we want to investigate */
    char strLocation[256];
	/* strDevice will contain the path to the actual device,
		 (several versions of it, with varying detail)
	    rather than the symbolic link */
    char strDevice[256];
	/* strId will contain the identifier for the device, that will be used by
		this code base */
	char strId[256];

	/* Construct the path to /sys/block/... */
	snprintf(strLocation, sizeof strLocation, "/sys/block/%s", strInputPath);
    
	/* Read symbolink link 'location' into buf, 
	   returns the number of bytes it has placed in buf */
    len = readlink(strLocation, buf, 256);
    buf[len] = 0;

	/* strDevice will contain the path to the actual device, 
	    rather than the symbolic link */
    sprintf(strDevice, "%s/%s", "/sys/block/", buf);

	/* Now we try to gather device information 
	    such as manufacturer, product and serial number.
	   strDevice will be changed in the process. 
	   We use a variable to make note of the length,
		to be sure that we can properly zero-terminate. */
	int intIdLen = 0;
	getDeviceInfo(strId, &intIdLen, strDevice);
	if(intIdLen==0)
		return; /* Some kind of bogus device */
	
	if(gModeSetting == LISTMODE)
	{	/* In list mode, we are only interested in listing the devices */
		printf("%s\n", strId);
		return;
	}

	if(checkIfItemMounted(strInputPath)==0)
		return; /* Not mounted, not of interest */
	processDevice(strId);
}

void getDeviceInfo(char* out, int *pOutLen, const char device[])
{
    int i;
	char *p;
	/* strModel will contain the identifier for the model of this device, 
		as reported by the device. 
      This is of interest in the case of multi-card readers. */
	char strModel[256];
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
	char source[512];
	char buf[512];

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
    int len = read(f, buf, 512);
	if (len <= 0)
        return;

	/* Update the out variables */
	*pOutLen += len;
	buf[len-1] = 0;
	strcat(out, (const char*) buf);
	tidyStringUp(out);
	*pOutLen = strlen(out) + 1;
}

int checkIfItemMounted(char *item)
{
	char outputBuffer[32];
	char newItemId[32];
	sprintf(newItemId, "/dev/%s", item);
	
	FILE *file = fopen ("/proc/mounts", "r");
	if (!file)
	{
		syslog(LOG_ERR, "Could not open /proc/mounts");
		exit(EXIT_FAILURE);
	}

	while(!feof(file)) {
		if (fgets(outputBuffer,sizeof(outputBuffer),file)) {
		    /* Check if this line starts with our item */
			if(strncmp(outputBuffer, newItemId, strlen(newItemId)) != 0)
				continue;
			/* It does */
			syslog(LOG_NOTICE, "%s has been mounted, according to /proc/mounts: %s", item, outputBuffer);
			return 1;
		}
	}
	return 0;
}
