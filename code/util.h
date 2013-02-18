/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#ifndef UTIL_H
#define UTIL_H

void tidyStringUp(char * string);
int contains(const char * const * list, const char* item);
char *replace(char *orig, char *rep, char *with);

#endif
