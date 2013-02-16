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
#include <ctype.h> // isspace()
#include "detection.h"
#include "bookKeeping.h"
#include "global.h"

void doCheck(void) 
{
    DIR *dirp;
    struct dirent *dp;
    char* dir_to_check = "/sys/block/";

	initializeRun();

	if ((dirp = opendir(dir_to_check)) == NULL) 
    {
		syslog(LOG_ERR, "couldn't open '%s'",  dir_to_check);  
        return;
    }
    do 
    {	// Proccess each item in /sys/block
        if ((dp = readdir(dirp)) == NULL) 
        	continue;
		if(dp->d_name[0]!='s' || dp->d_name[1]!='d')
        	continue; //sda, sdb, sdc, ...
        processItem(dp->d_name);
    } while (dp != NULL);

	finalizeRun();
}

void processItem(char *strInputPath) 
{
    ssize_t len; // Length of the path to the symbolic link
    int i;
    char buf[256], *p;
    char strDevice[256];
    char strLocation[256];
	char strId[256];

	// Construct the path to /sys/block/...
	snprintf(strLocation, sizeof strLocation, "/sys/block/%s", strInputPath);
    
	// Read symbolink link 'location' into buf, 
	// returns the number of bytes it has placed in buf
    len = readlink(strLocation, buf, 256);
    buf[len] = 0;

	// Format a string, into device
	// device will contain the path to the actual device, 
	// rather than the symbolic link
    sprintf(strDevice, "%s/%s", "/sys/block/", buf);

	// strrchr: Locate last occurrence of character in string
	//          Returns a pointer to the last occurrence of 
	//			character in the C string str.
	// In effect, we drop everything after and including the sixth-from-the-back
	// '/' character
	// We do this to get the proper device location
    for (i=0; i<6; i++) {
        p = strrchr(strDevice, '/');
        *p = 0;
    }
	
	// Now we try to create a device id
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
	addDeviceInfo(out, pOutLen, device, "/manufacturer");
	if (pOutLen > 0)
		out[*pOutLen-1] = ' ';
	addDeviceInfo(out, pOutLen, device, "/product");
	if (pOutLen > 0)
		out[*pOutLen-1] = ' ';
	addDeviceInfo(out, pOutLen, device, "/serial");
	if (pOutLen <= 0)
		return;

	// Remove trailing spaces
	i = strlen(out);
	for(; i > -1; i--)
	{
		if(!isspace(out[i]) && !isblank(out[i]) && !iscntrl(out[i]))
			break;
		out[i] = '\0';
	}
}

void addDeviceInfo(char *out, int *pOutLen, const char device[], const char property[])
{
	char source[512];
	char buf[512];

	// Make sure that out has a null character to terminate it, 
	// this is necessary when using strcat
	out[*pOutLen] = 0;

	// Constructing source of desired info
	strcpy(source, device);
    strcat(source, property);

	// Concatenating desired info to target,
	// if such info appears to be found
	int f = open(source, 0);
    int len = read(f, buf, 512);
	if (len <= 0)
        return;
	*pOutLen += len;
	buf[len-1] = 0;
	strcat(out, (const char*) buf);
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
		    // Check if this line starts with our item
			if(strncmp(outputBuffer, newItemId, strlen(newItemId)) != 0)
				continue;
			// It does
			syslog(LOG_NOTICE, "%s has been mounted, according to /proc/mounts: %s", item, outputBuffer);
			return 1;
		}
	}
	return 0;
}
