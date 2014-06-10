/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#include <stdlib.h>
#include <string.h>
#include <stdio.h> /* util.h uses FILE */
#include <dirent.h> /* util.h uses DIR */
#include <syslog.h>
#include <getopt.h>
#include "bookKeeping.h"
#include "global.h"
#include "processArguments.h"
#include "util.h"

const char* signalTarget; /* declared in processArguments.h */
const char* gTargetDirectory; /* declared in processArguments.h */
const char* signalWhitelist; /* declared in processArguments.h */
char **gBlacklist; /* declared in global.h */
const char* signalBlacklist; /* declared in processArguments.h */
char **gWhitelist; /* declared in global.h */

/* Which type of notification should be used */
int gNotificationSetting; /* declared in global.h */
const char* signalNotificationSetting; /* declared in processArguments.h */
const char* signalNotifySend; /* declared in processArguments.h */
const char* signalXMessage; /* declared in processArguments.h */

void initializeArgumentProcessing()
{
    signalTarget = "--target";
    signalBlacklist = "--blacklist";
    signalWhitelist = "--whitelist";
    gBlacklist = calloc(MAXLISTLENGTH, MAXIDLENGTH);
    gWhitelist = calloc(MAXLISTLENGTH, MAXIDLENGTH);
    
    gNotificationSetting = NONOTIFICATIONS;
    signalNotificationSetting = "--notification";
    signalNotifySend = "notify-send";
    signalXMessage = "xmessage";
}

void processArguments(int argc, char * const *argv)
{
    int caseIndex;
    static struct option options[] =
        {
            {"target",       required_argument, 0, 't'},
            {"blacklist",    required_argument, 0, 'b'},
            {"whitelist",    required_argument, 0, 'w'},
            {"notification", required_argument, 0, 'n'},
            {0, 0, 0, 0}
        };

    initializeArgumentProcessing();
    
    while (1)
    {
        /* getopt_long stores the option index here. */
        int optIndex = 0;

        caseIndex = getopt_long (argc, argv, "t:b:w:n:", options, &optIndex);

        /* Detect the end of the options. */
        if (caseIndex == -1)
            break;

        switch (caseIndex)
        {
            case 't':
                syslog(LOG_DEBUG, "option -t with value `%s'", optarg);
                setTarget(optarg);
                break;

            case 'b':
                syslog(LOG_DEBUG, "option -b with value `%s'", optarg);
                loadListFromFile(gBlacklist, optarg);
                break;

            case 'w':
                syslog(LOG_DEBUG, "option -w with value `%s'", optarg);
                loadListFromFile(gWhitelist, optarg);
                break;

            case 'n':
                syslog(LOG_DEBUG, "option -n with value `%s'", optarg);
                setNotificationMode(optarg);
                break;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }
}

/* The target directory is being set */
void setTarget(char* optarg)
{
    if(gTargetDirectory != NULL)
    {
        logArgumentReSetIgnored(signalTarget);
        return;
    }
    gTargetDirectory = optarg;
    syslog(LOG_INFO, "Target has been set to \"%s\".\n", optarg);
}

/* Open a file, and copy each line into the memory behind one of the pointers in out */
void loadListFromFile(char **out, char *optarg)
{    
    
    FILE *file = openOrDie(optarg, "r"); /* util.c */
    int count = 0;
    char outputBuffer[MAXIDLENGTH];
    while(!feof(file)) {
        if (fgets(outputBuffer, MAXIDLENGTH, file)) {
            tidyStringUp(outputBuffer);
            out[count] = malloc(MAXIDLENGTH);
            if(!out[count])
            {
                syslog(LOG_ERR, "Exiting with failure: malloc failed while processing arguments");
                exit(EXIT_FAILURE);
            }
            memcpy(out[count], outputBuffer, MAXIDLENGTH);
            out[count][MAXIDLENGTH -1] = '\0';
            count++;
        }
    }
    fclose(file);
    syslog(LOG_INFO, "Using list located at %s", optarg); 
}

void setNotificationMode(char *optarg)
{
    syslog(LOG_DEBUG, "setNotificationMode %s", optarg); 
    
    if(gNotificationSetting != NONOTIFICATIONS)
    {
        logArgumentReSetIgnored(signalNotificationSetting);
        return;
    }
    
    if(strcmp(optarg, signalNotifySend) == 0)
    {
        gNotificationSetting = NOTIFYSENDMODE;
        syslog(LOG_INFO, "Notifying user with notify-send"); 
        return;
    }
    if(strcmp(optarg, signalXMessage) == 0)
    {
        gNotificationSetting = XMESSAGEMODE;
        syslog(LOG_INFO, "Notifying user with xmessage"); 
        return;
    }
    
    syslog(LOG_INFO, 
            "--notification argument is not a known argument: %s", optarg); 
}

/* There was more than one attempt to set a specific argument. 
   The subsequent attempts should be ignored.
   Log that a re-set has been ignored. */
void logArgumentReSetIgnored(const char* argument)
{
    syslog(LOG_NOTICE, "There was an attempt to set the command-line argument \"%s\" more than once. The new value has been ignored.", argument);
}