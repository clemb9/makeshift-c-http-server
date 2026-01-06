// client.c
// sample client for server testing, not an actual http client
#include"utils.h"

int main(int argc, char **argv)
{
	char *buf = {"aaaa\nbbbbb\n"};
	char *ptr = buf;
	
	int line_size;

	line_size = get_linesize(ptr);
	printf("%d\n", line_size);	
	printf("%s\n", ptr);
	
	return 0;
}
