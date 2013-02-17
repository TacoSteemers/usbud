#ifndef USB_H
#define USB_H

void doCheck(void);
void processItem(char *devicePath, char* entryName);
int checkIfItemMounted(char *devicePath);
void getMountPoint(char *out, char *devicePath);
void getDeviceInfo(char* out, int *pOutLen, const char device[]);
void addDeviceInfo(char *id, int *p_len, const char *source, const char *property);

#endif
