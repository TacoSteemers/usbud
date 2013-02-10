#ifndef PROCESSARGUMENTS_H
#define PROCESSARGUMENTS_H

void processArguments(const char * const *arr, int arrc);
void initializeArgumentProcessing();
void setTarget(const char * const *arr, int arrc, int indexOfArgument);
int getArgumentSwitchInt(const char* argument);
void logArgumentReSetIgnored(const char* argument);

extern const char* signalTarget; /* defined in processArguments.c */

#endif
