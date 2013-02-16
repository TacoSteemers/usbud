#include <ctype.h>  /* isspace */
#include <string.h> /* strlen */
#include "global.h" /* MAXLISTLENGTH */

/* Removes trailing spaces, blanks and control characters */
void tidyStringUp(char * string)
{
	int i;
	i = strlen(string);
	for(; i > -1; i--)
	{
		if(!isspace(string[i]) && !isblank(string[i]) && !iscntrl(string[i]))
			break;
		string[i] = '\0';
	}
}

int contains(const char * const * list, const char* item)
{
	int i = 0;
	for(; i < MAXLISTLENGTH; i++)
	{
		if(!list[i] || list[i][0] == '\0')
			return 0; /* No match, reached end of list contents */
		if( (strncmp(list[i], item, strlen(item)) == 0) && 
			(strlen(list[i]) == strlen(item)))
			return 1; /* This is a match */
	}
	return 0; /* No match */
}
