#ifndef PROCESSARGUMENTS_H
#define PROCESSARGUMENTS_H

void processArguments(const char * const *arr, int arrc);
void initializeArgumentProcessing(void);
void setTarget(const char * const *arr, int arrc, int indexOfArgument);
int getArgumentSwitchInt(const char* argument);
void logArgumentReSetIgnored(const char* argument);
void loadListFromFile(char **out, const char * const *arr, int arrc, int indexOfArgument);

const char* signalTarget; /* defined in processArguments.c */
const char* signalBlacklist; /* defined in processArguments.c */
const char* signalWhitelist; /* defined in processArguments.c */

#endif
