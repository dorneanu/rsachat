#include <stdio.h>
#include <string.h>


/*
 * Same effect as fgets but we overwrite the '\n' by '\0'
 */
char *mygetline(char *line, int size)
{
	if (fgets(line, size, stdin)) {
		char *newline = strchr(line, '\n'); /* check for trailing '\n' */
		if (newline) {
			*newline = '\0'; /* overwrite the '\n' with a terminating null */
		}
	}
	return line;
}
