#include <ctype.h> // isspace
#include <string.h> // strlen

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
