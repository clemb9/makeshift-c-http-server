// responses.c
#include "utils.h"

// Utils
char *get_date_hdr(char *dst) {
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);					// Get raw time
	timeinfo = gmtime(&rawtime);	// & convert to GMT
	strftime(dst + strlen(dst), 64, "Date: %a, %d %b %Y %H:%M:%S GMT\n", timeinfo);

	return dst;
}


int get_content_length(char *path_to_ressource)
{
	int	fd;
	int n, sum;
	char buffer[1024];
	fd = Open(path_to_ressource, O_RDONLY);
	
	sum = 0;
	while ( (n = Read(fd, buffer, 1024)) )
		{
		sum+=n;	
		}

	return sum;
}



void get_content_type(char *dst, char *ressource_path)
{
	const char 	*mime;
	magic_t 	magic;

	if ( (magic = magic_open(MAGIC_MIME_TYPE)) == NULL)
	{
		printf("Error creating magic cookie: %s", magic_error(magic));
		return;
	}
	
	magic_load(magic, NULL);
	
	if ( (mime = magic_file(magic, ressource_path)) == NULL)
	{
		printf("Error loading file %s: %s", ressource_path, magic_error(magic));
		return;
	}
	sprintf(dst, mime);
	magic_close(magic);
}

char *prep_response(char *dst, char *path_to_ressource) // Append server, date, info, length... to header equiped response
	{	
	char 	type[64];
	int 	length;
	
	get_date_hdr(dst);
	get_content_type(type, path_to_ressource);
	length = get_content_length(path_to_ressource);
	
	snprintf(dst + strlen(dst), 128, "Server: Homemade Linux Webserver\nContent-Type: %s\n",type);
	if (length > 0) 
			snprintf(dst + strlen(dst), 64, "Content-Length: %d\n\n", length);
	
	return dst;
	}



// --- 20X responsee
void resp_get(int sockfd, char *ressource_path)
{
	char	reply[MAX_LINE];
	char 	buffer[MAX_BUF];
	int 	fd, n;
	
	// Prep header
	snprintf(reply, 64, "HTTP/1.1 200 OK\n");
	prep_response(reply, ressource_path);
	// Prep file
	fd = Open(ressource_path, O_RDONLY);
	// Send response
	printf("[.] Reply header: %s\n", reply);
	Write(sockfd, reply, strlen(reply));
	while (n = Read(fd, buffer, MAX_BUF))
	{
		Writen(sockfd, buffer, n);	
	}	
}

// --- 40X responses
void resp_notfound(int sockfd) // Err 404
{	
	char 	reply[MAX_LINE];
	char 	buffer[MAX_BUF];
	int		fd;
	
	// Prep header
	snprintf(reply, 64, "HTTP/1.1 404 Not Found\n");
	prep_response(reply, "../www/notfound.html");
	// Prep file
	fd = Open("../www/notfound.html", O_RDONLY);
	// Send response
	Write(sockfd, reply, strlen(reply)); // Send the header
	printf("[.] Reply header:\n%s", reply);	
	while (Read(fd, buffer, MAX_BUF)) // Send body
	{
		Write(sockfd, buffer, strlen(buffer));	
	}	
}

void resp_invalid(int sockfd) // Err 400
{
	char 	reply[MAX_LINE];
	char 	buffer[MAX_BUF];
	int 	fd;
	
	// Prep header
	snprintf(reply, 70, "HTTP/1.1 401 Bad Request\n");
	
	prep_response(reply, "../www/badreq.html");
	// Prep file
	fd = Open("../www/badreq.html", O_RDONLY); // Send the header
	Write(sockfd, reply, strlen(reply));
	printf("[.] Reply: header\n%s", reply);	
	while (Read(fd, buffer, MAX_BUF))
	{
		Write(sockfd, buffer, strlen(buffer));	
	}	
	// Send the body
}


