#include "fileread.h"

// fgetstr() - mimics behavior of fgets(), but removes new-line
// character at end of line if it exists
char *fgetstr(char *string, int n, FILE *stream)
{
	char *result;
	result = fgets(string, n, stream);
	if(!result)
		return(result);

	if(string[strlen(string) - 1] == '\n')
		string[strlen(string) - 1] = '\0';

	return(string);
}

