/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#ifndef BACKUP_H
#define BACKUP_H

void processDevice(char *mountPoint, char *deviceId);
int needsBackup(char* deviceId);
void createTargetDirectoryString(char* output, char* deviceId);
void doBackup(char *source, char *target);

#endif
