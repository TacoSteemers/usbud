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
#include "usb.h"

void doCheck(void) {
    DIR *dirp;
    struct dirent *dp;
    char dir_to_check[256];

    snprintf(dir_to_check, sizeof dir_to_check, "/sys/block/");
	if ((dirp = opendir("/sys/block/")) == NULL) 
    {
		syslog(LOG_ERR, "couldn't open '/sys/block/'");  
        return;
    }


	// Proccess each item in /sys/block
    do 
    {
        if ((dp = readdir(dirp)) != NULL) 
        {
            char location[256];
            snprintf(location, sizeof location, "%s%s", dir_to_check, dp->d_name);

			if(dp->d_name[0]=='s' && dp->d_name[1]=='d')
            {
                processItem(dp->d_name);
            }
        }
    } while (dp != NULL);
}

void processItem(char *str) {
	//syslog(LOG_DEBUG, "looking at %s\n", str); 

    ssize_t len; // Length of the ppath to the symbolic link
    int i;
    char buf[256], *p;
    char device[256];
    char location[256];
	char id[1024];

	// Construct the path to /sys/block/...
	snprintf(location, sizeof location, "/sys/block/%s", str);
    
	// Read symbolink link 'location' into buf, 
	// returns the number of bytes it has placed in buf
    len = readlink(location, buf, 256);
    buf[len] = 0;
	//syslog(LOG_DEBUG, "buf\t%s\n", buf); 
	

	// Format a string, into device
	// device will contain the path to the actual device, 
	// rather than the symbolic link
    sprintf(device, "%s/%s", "/sys/block/", buf);
	//syslog(LOG_DEBUG, "device first\t%s\n", device); 

	// strrchr: Locate last occurrence of character in string
	//          Returns a pointer to the last occurrence of 
	//			character in the C string str.
	// In effect, we drop everything after and including the sixth-from-the-back
	// '/' character
	// We do this to get the proper device location
    for (i=0; i<6; i++) {
        p = strrchr(device, '/');
        *p = 0;
    }
	//syslog(LOG_DEBUG, "device second\t%s\n", device); 

	
	// Now we try to create a device id
	int intIdLen = 0;
	getDeviceInfo(id, &intIdLen, device);
	if(intIdLen!=0)
	{
		//syslog(LOG_INFO, "found %s in spot %s\n", id, location);
		checkIfItemMounted(str) ;
	}
}

void getDeviceInfo(char* out, int *pOutLen, const char device[]){
	addDeviceInfo(out, pOutLen, device, "/manufacturer");
	if (pOutLen > 0)
	{
		out[*pOutLen-1] = ' ';
	} 
	addDeviceInfo(out, pOutLen, device, "/product");
	if (pOutLen > 0)
	{
		out[*pOutLen-1] = ' ';
	} 
	addDeviceInfo(out, pOutLen, device, "/serial");
	if (pOutLen == 0)
	{
		strcpy(out, "");
	} 
	else
	{
		//out[*pOutLen-1] = 0;
	}
}

void addDeviceInfo(char *out, int *pOutLen, const char device[], const char property[]){
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
	if (len <= 0) {
        return;
    }
	*pOutLen += len;
	buf[len-1] = 0;
	strcat(out, (const char*) buf);
}

int checkIfItemMounted(char *item){
	char outputBuffer[32];
	char newItemId[32];
	sprintf(newItemId, "/dev/%s", item);
	
	FILE *file = fopen ("/proc/mounts", "r");
	if (!file)
	{
		syslog(LOG_ERR, "Could not open /proc/mounts");
		return EXIT_FAILURE;
	}

	while(!feof(file)) {
		if (fgets(outputBuffer,sizeof(outputBuffer),file)) {
		    // Check if this line starts with our item
			if(strncmp(outputBuffer, newItemId, strlen(newItemId)) != 0)
			{
				continue;
			}
			// It does
			syslog(LOG_NOTICE, "%s has been mounted, according to /proc/mounts: %s", item, outputBuffer);
			return 1;
		}
	}
	return 0;
}
