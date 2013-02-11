#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include "devices.h"
#include "global.h"
#include "processArguments.h"

const char* signalTarget; /* declared in processArguments.h */
const char* gTargetDirectory; /* declared in processArguments.h */

void initializeArgumentProcessing()
{
	signalTarget = "--target";
}

void processArguments(const char * const *arr, int arrc)
{
    int i = 0;
	const char* argument;
	syslog(LOG_DEBUG, "Entered the processArguments method.\n"); 

	if(arrc == 1)
	{
		syslog(LOG_DEBUG, "Leaving the processArguments method. There are no arguments to process.\n");
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
			case -1:
				syslog(LOG_ERR, "Exiting with failure. A null argument was passed to getArgumentIndex.\n");
		        exit(EXIT_FAILURE);
				break;
			case -2:
			default :
				syslog(LOG_ERR, "Unknown command-line argument: %s .\n", argument);
		}
	}
	syslog(LOG_DEBUG, "Leaving the processArguments method.\n"); 
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
	return -2;
}

/* The target directory is being set */
void setTarget(const char * const *arr, int arrc, int indexOfArgument)
{
	syslog(LOG_DEBUG, "Entered the setTarget method.\n"); 
	if((indexOfArgument + 1) >= arrc)
	{
		syslog(LOG_ERR, "Exiting with failure. Command-line argument \"%s\" can not be set: it is missing it's parameter. %d %d\n", signalTarget, arrc, indexOfArgument);
        exit(EXIT_FAILURE);
	}
    if(gTargetDirectory != NULL)
	{
		logArgumentReSetIgnored(signalTarget);
		return;
	}
	gTargetDirectory = arr[indexOfArgument + 1];
	syslog(LOG_DEBUG, "Target has been set to \"%s\".\n", gTargetDirectory); 
	syslog(LOG_DEBUG, "Leaving the setTarget method.\n"); 
}

void logArgumentReSetIgnored(const char* argument)
{
	syslog(LOG_NOTICE, "An attempt to set the command-line argument \"%s\" after it has already been set, has been ignored.\n", argument);
}
