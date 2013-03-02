/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#include <stdlib.h>
#include <string.h>
#include <stdio.h> /* util.h uses FILE */
#include <dirent.h> /* util.h uses DIR */
#include "logging.h"
#include "bookKeeping.h"
#include "global.h"
#include "processArguments.h"
#include "util.h"

const char* signalTarget; /* declared in processArguments.h */
const char* gTargetDirectory; /* declared in processArguments.h */
const char * signalWhitelist; /* declared in processArguments.h */
char **gBlacklist; /* declared in global.h */
const char * signalBlacklist; /* declared in processArguments.h */
char **gWhitelist; /* declared in global.h */

void initializeArgumentProcessing()
{
	signalTarget = "--target";
	signalBlacklist = "--blacklist";
	signalWhitelist = "--whitelist";
	gBlacklist = malloc(MAXLISTLENGTH);
	gWhitelist = malloc(MAXLISTLENGTH);
}

void processArguments(const char * const *arr, int arrc)
{
    int i = 1;
	const char* argument;

	if(arrc == 1)
	{
		syslog(LOG_DEBUG, "There are no arguments to process.");
		return;
	}

	initializeArgumentProcessing();

    for(; i < arrc; i++)
	{
		argument = arr[i];
		switch( getArgumentSwitchInt(argument) ) 
		{
			case 0:
				setTarget(arr, arrc, i);
				i = i + 1; /* skip past argument parameter */
				break;
			case 1:
				loadListFromFile(gBlacklist, arr, arrc, i);
				i = i + 1; /* skip past argument parameter */
				break;
			case 2:
				loadListFromFile(gWhitelist, arr, arrc, i);
				i = i + 1; /* skip past argument parameter */
				break;
			case -1:
				syslog(LOG_ERR, "Exiting with failure. A null argument was passed to getArgumentIndex.");
		        exit(EXIT_FAILURE);
				break;
			case -2:
			default :
				syslog(LOG_ERR, "Unknown command-line argument: %s", argument);
		}
	} 
}

/* Translate a command-line argument to an int, for use in a switch function */
int getArgumentSwitchInt(const char* argument) 
{
	if(argument == NULL)
	{
		return -1;		
	}
	if(strcmp(argument, signalTarget) == 0)
	{	/* The target directory is being set */
		return 0;
	}
	if(strcmp(argument, signalBlacklist) == 0)
	{	/* The blacklist filepath is being set */
		return 1;
	}
	if(strcmp(argument, signalWhitelist) == 0)
	{	/* The whitelist filepath is being set */
		return 2;
	}
	return -2;
}

/* The target directory is being set */
void setTarget(const char * const *arr, int arrc, int indexOfArgument)
{
	if((indexOfArgument + 1) >= arrc)
	{
		syslog(LOG_ERR, "Exiting with failure. Command-line argument \"%s\" can not be set: it is missing it's parameter.", signalTarget);
        exit(EXIT_FAILURE);
	}
    if(gTargetDirectory != NULL)
	{
		logArgumentReSetIgnored(signalTarget);
		return;
	}
	gTargetDirectory = arr[indexOfArgument + 1];
	syslog(LOG_INFO, "Target has been set to \"%s\".\n", gTargetDirectory);
}

void logArgumentReSetIgnored(const char* argument)
{
	syslog(LOG_NOTICE, "An attempt to set the command-line argument \"%s\" after it has already been set, has been ignored.", argument);
}

/* Open a file, and copy each line into the memory behind one of the pointers in out */
void loadListFromFile(char **out, const char * const *arr, int arrc, int indexOfArgument)
{	
	if((indexOfArgument + 1) >= arrc)
	{
		syslog(LOG_ERR, "Exiting with failure. Command-line argument \"%s\" can not be set: it is missing it's parameter.", arr[indexOfArgument]);
        exit(EXIT_FAILURE);
	}
	FILE *file = openOrDie(arr[indexOfArgument + 1], "r"); /* util.c */
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
	syslog(LOG_INFO, "Using list located at \"%s\".\n", arr[indexOfArgument + 1]); 
}
