/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#ifndef USB_H
#define USB_H

void doCheck(void);
void processPartition(char *devicePath, 
    char *deviceName, char *partition);
int checkIfItemMounted(char *devicePath);
void getMountPoint(char *out, char *devicePath);
void getDeviceInfo(char* out, int *pOutLen, const char device[]);
void addDeviceInfo(char *id, int *p_len, 
    const char *source, const char *property);
char* findMountedPartitions(char* output[], 
    int* pCount, char *devicePath);

#endif
