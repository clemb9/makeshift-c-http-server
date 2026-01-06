//error.c
#include"utils.h"



void err_print()
{
	char 	buffer[MAX_LINE];
	int 	err_save;
	
	err_save = errno;
	snprintf(buffer, sizeof(buffer), ": %s\n", strerror(err_save));

	fputs(buffer, stdout); 
}
