/* USB Storage Backup Daemon
   Copyright 2012, Taco steemers. This program is distributed under the terms of
   the GNU General Public License. A version of this license should have been 
   provided. It can also be found on: http://www.gnu.org/licenses/gpl-3.0.txt */

#include <stdlib.h> /* malloc and such */
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
		if(list[i] == NULL || list[i][0] == '\0')
			return 0; /* No match, reached end of list contents */
		if( (strncmp(list[i], item, strlen(item)) == 0) && 
			(strlen(list[i]) == strlen(item)))
			return 1; /* This is a match */
	}
	return 0; /* No match */
}

/* Following substring replacement code is written by jmucchiello ,and found 
	here: http://stackoverflow.com/questions/779875/
   As such, it's licensing is as follows: 
	"user contributed content licensed under cc-wiki with attribution required"
   This licence primarily concerns itself with attribution and sharing.

   Author's note:
   You must free the result if result is non-NULL. */
char *replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    if (!orig)
        return NULL;
    if (!rep || !(len_rep = strlen(rep)))
        return NULL;
    if (!(ins = strstr(orig, rep))) 
        return NULL;
    if (!with)
        with = "";
    len_with = strlen(with);

    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}
