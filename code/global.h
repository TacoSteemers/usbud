#ifndef GLOBAL_H
#define GLOBAL_H

const char *gTargetDirectory; /* defined in processArguments.c */
int gCurrentRunId; /* defined in daemon.c */

const int gModeSetting; /* Defined in either daemon.c or uslist.c */
#define DAEMONMODE 1
#define LISTMODE 2

#define MAXLISTLENGTH 256
#define MAXIDLENGTH 256
char **gBlacklist; /* Defined in processArguments.c*/
char **gWhitelist; /* Defined in processArguments.c*/
#endif
