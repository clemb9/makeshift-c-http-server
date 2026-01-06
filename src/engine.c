// engine.c
// Application related functions and helpers
#include"utils.h"

// concerned with working with case incensitive strings
// int flag: conver TO lowercase or TO uppercase
int parse_verb(char *verb)
	{
	int		result = INVALID ; // equals zero

	if ( 0 == strcmp(verb, "get") )
		result = GET;
	if ( 0 == strcmp(verb, "post") )
		result = POST;
	if (0 == strcmp(verb, "head") )
		result = HEAD;
	if (0 == strcmp(verb, "put") )
		result = PUT;
	if (0 == strcmp(verb, "delete") )
		result = DELETE;
	if (0 == strcmp(verb, "connect") )
		result = CONNECT;
	if (0 == strcmp(verb, "options") )
		result = OPTIONS;	
	if (0 == strcmp(verb, "trace") )
		result = TRACE;
	if (0 == strcmp(verb, "patch") )
		result = PATCH;
	
	return result;
	}

int get_ressource_path(char *name, char *dst, struct rf_table *lkp_table)
{
	int 	n;
	char 	*ptr = lkp_table->table;

	int		increment = 0;
	
	for (;;)
	{	
		if ( strncmp(name, ptr, strlen(name) ) ) // if it doesn't return 0: different strings, not on this line
		{
			increment = get_linesize(ptr);
			if (increment < 0) // reached end of string
			{
				break; 		
			}
			

		}	
	}	
	
	return -1;
}

char *convert_case(char *str, int n, int flag)
{
	size_t 	i;
	// want to loop over the string and convert it all to either lowercase or upper case
	for (i=0; i<n; i++)
	{
		char letter = str[i];
	
		if (flag==LOWER)
		{
			
			if (letter >= 'A' && letter <= 'Z')
					letter += 32;	
		}
		else if (flag==UPPER)
		{
			if (letter >= 'a' && letter <= 'z')
					letter -=32;
		}
		str[i] = letter;
	}
	return str;
}

// request as a bitstring 
// 1 GET
// 2 POST

size_t get_linesize(char *buffer)
{
	int i = 0;

	for ( ; ; ) 
	{
		if ( (buffer[i] == '\n') | ( (strncmp(buffer+i, "\r\n", 2)) == 0) | (buffer[i] == '\0') ) 
			return i;
			i++;
	}

	// assumes buffer is null terminated
}




void serve(int sockfd, struct rf_table *lkp_table)
{
	int 	n, n_read, line_size, hdrs_flag, head_size;
	int		line_index;
	int 	verb;
	char 	buf[MAX_REQ];
	char	line[MAX_LINE];
	char 	ressource_name[MAX_LINE];
	char 	ressource_path[MAX_LINE];

	char 	*bufptr;
	struct 	params	param_list;
	
start:		
	char 	*token;
	
	bufptr				= buf;
	buf[0] 				= '\0';	
	ressource_name[0] 	= '\0';
	ressource_path[0]	= '\0';
	verb	 			= 0;
	hdrs_flag 			= 0;
	n_read 				= 0;
	line_index 			= 0;
	bzero(&param_list, sizeof(param_list));

	// Read the headers
	while (0 == hdrs_flag)
	{
		// Read the entire headers from the request
		
		n = read(sockfd, bufptr+n_read, MAX_REQ);
		n_read += n;
		printf("just read %d bytes from socket\n", n);
		if (n == -1)
		{	
			if (errno == EINTR)
			{
				n = 0;
				continue;
			}
			
			else 
			{
				printf("read error");
				err_print();
				exit(1);
			}
		}

		if (n == 0)
		{
			printf("connection terminated\n");
			return;
		}

		for (int i=0; i < n_read; i++)
		{
			if (strncmp(bufptr+i, "\r\n\r\n", 4) == 0)
			{
				hdrs_flag = 1; // all the headers have been read in the buffer
				head_size = i;
				break;
			}	
		}
	}
	// Printing the request, debugging purpose
	//fputs(bufptr, stdout);
	
	// [-] Ignore content for now.
	
	// [--] Process request and respond
	 
	// [---] Process first line
	line_size = get_linesize(bufptr);
	
	// If the first line is also the last, starts with a newline or the buffer is empty
	if ( (line_size < 0) | (line_size == 0) )
	{
		resp_invalid(sockfd);
		goto start;
	}

	line_index += line_size; 
	strncpy(line, bufptr, line_size);
	convert_case(line, line_size, LOWER);
	
	// [----] Save request verb	
	token = strtok(line, " ");
	verb = parse_verb(token);	
	
	// [----] Save ressource name
	if (verb == GET)
		{	
		token = strtok(NULL, " ");
		strcpy(ressource_name, token);
		}
	printf("processing headers...\n");
	// Iterate over the next parameters		
	if ( (n = process_headers(bufptr+line_index, &param_list) ) == -1) // Return 400, no header
		{
		resp_invalid(sockfd);						
		goto start;
		}
	
	if (verb == GET)
	{
		if ( (n = get_ressource_path(ressource_name, ressource_path, lkp_table)) ==-1 )
		{
			resp_notfound(sockfd);
			goto start;
		}
		printf("[.] Ressource requested: %s at path:%s\n", ressource_name, ressource_path);
		resp_get(sockfd, ressource_path);	
	}
	goto start;
}


// Process headers is passed a pointer to headers from the request (without the body)
size_t process_headers(char *request, struct params *param_list)
{
	int line_size;
	int	n = -1;
	int running = 1;
	char line[MAX_LINE];
	
	printf("processing headers from request: %s", request);

	printf("%d", request[0]);

	while (running)
	{
		// Adjust for the newline character(s) after line_size increment
		if (request[0] == '\n') 
		{	
			request++;
			printf("adding padding..\n");
		}
		if (strncmp(request, "\r\n", 2) == 0)
		{request+=2;
			printf("pushing");	
		}
		line[0] = '\0';
		
		if ( (line_size = (get_linesize(request))) == 0 ) // Have encountered the double newline that terminates the request headers
			break;	
		
		if (line_size < 0) // Reached end of buffer, somehow not terminated with double new line ; server accepts it
			running = 0;

		strncpy(line, request, line_size);
		convert_case(line, line_size, LOWER);
		
		if (strncmp(line, "host:", 5) == 0)
		{
			// Host is set, package has reached, don't care about rest for now.
			n = 1;
		}

		if (strncmp(line, "content-length:", 15) == 0)
		{
			int diff = line_size - 15;	
			if (diff <= 0) // If header present but not specified
				return -1;		
		}
	
		request += line_size; 
		printf("looping over...");
	}	
	return n;
}

void print_help_exit()
{
	printf("[.] Usage:\n\tDefault: ./server\n\tCustom parameters: ./server [path to parameter file]\n\tHelp: ./server -h\n");
	exit(0);
}

void load_config(char *path_to_config, struct rf_table *lkp_table)
{
	int 	fd, n, size, nb_lines;
	char	buf[MAX_BUF];
	size = 0;

	nb_lines = 0;
	fd = Open(path_to_config, O_RDONLY);		
	while ( (n = Read(fd, buf, MAX_BUF)) > 0) // Counting the number of lines and filesize in bytes in one go.
	{
		for (int i=0; i<n; ++i)
		{
			if ( buf[i] == '\n' ) 
				nb_lines +=1;
		}
		size += n;		
	}	
	Close(fd);	

	if ( (lkp_table->table = malloc(size)) == NULL)
	{
		printf("malloc error");
		err_print();
		exit(1);	
	}
	// Read the file again.
	// Copy the content of the file into memory.
	lkp_table->n 			= nb_lines;
	lkp_table->table_len 	= size; 

	lkp_table->table[0] = '\0';
	fd = Open(path_to_config, O_RDONLY);
	while ( (n = Read(fd, buf, size)) > 0)
	{
		strncpy(lkp_table->table + strlen(lkp_table->table), buf, size);
	}
	Close(fd);
	printf("[.] Ressources loaded: %s", lkp_table->table);

}

void process_parameters(int argc, char **argv, struct rf_table *lkp_table)
{
	if (argc == 1) // If no argument
	{
		printf("[.] Loading default ressources\n");
		load_config("../config/default.txt", lkp_table);		
	}
	else // If some CL argument
	{	
		if (argc > 2)
			print_help_exit();
		if (strncmp(argv[1], "-h", 2) == 0)
			print_help_exit();	
		else
		{
			load_config(argv[1], lkp_table);
		}
	}
	
	printf("[.] Ressources: %s\n", lkp_table->table);
}



// Notes for later:
/* It is RECOMMENDED that all senders and recipients support, at a minimum, URIs with lengths of 8000 octets in protocol elements. 
 * Note that this implies some structures and on-wire representations 
 * (for example, the request line in HTTP/1.1) will necessarily be larger in some cases. 
 *A sender MUST NOT generate an "http" URI with an empty host identifier. A recipient that processes such a URI reference MUST reject it as invalid.
 
 - If the port is equal to the default port for a scheme, the normal form is to omit the port subcomponent.

 - When not being used as the target of an OPTIONS request, an empty path component is equivalent to an absolute 
 path of "/", so the normal form is to provide a path of "/" instead.

 - The scheme and host are case-insensitive and normally provided in lowercase; all other components are compared in a case-sensitive manner.

 - Characters other than those in the "reserved" set are equivalent to their percent-encoded octets: the normal form is to not encode them (see Sections 2.1 and 2.2 of [URI]).

Note: In practice, the "Set-Cookie" header field ([COOKIE]) often appears in a response message across multiple field lines and does not use the list syntax, violating the above requirements on multiple field lines with the same field name. Since it cannot be combined into a single field value, recipients ought to handle "Set-Cookie" as a special case while processing fields. (See Appendix A.2.3 of [Kri2001] for details.)

The order in which field lines with differing field names are received in a section is not significant. However, it is good practice to send header fields that contain additional control data first, such as Host on requests and Date on responses, so that implementations can decide when not to handle a message as early as possible.

A server that receives a request header field line, field value, or set of fields larger than it wishes to process MUST respond with an appropriate 4xx (Client Error) status code. Ignoring such header fields would increase the server's vulnerability to request smuggling attacks (Section 11.2 of [HTTP/1.1]).


A field value does not include leading or trailing whitespace. When a specific version of HTTP allows such whitespace to appear in a message, a field parsing implementation MUST exclude such whitespace prior to evaluating the field value.

Field values are usually constrained to the range of US-ASCII characters [USASCII]. Fields needing a greater range of characters can use an encoding, such as the one defined in [RFC8187]. Historically, HTTP allowed field content with text in the ISO-8859-1 charset [ISO-8859-1], supporting other charsets only through use of [RFC2047] encoding. Specifications for newly defined fields SHOULD limit their values to visible US-ASCII octets (VCHAR), SP, and HTAB. A recipient SHOULD treat other allowed octets in field content (i.e., obs-text) as opaque data. Field values containing CR, LF, or NUL characters are invalid and dangerous, due to the varying ways that implementations might parse and interpret those characters; a recipient of CR, LF, or NUL within a field value MUST either reject the message or replace each of those characters with SP before further processing or forwarding of that message. Field values containing other CTL characters are also invalid; however, recipients MAY retain such characters for the sake of robustness when they appear within a safe context (e.g., an application-specific quoted string that will not be processed by any downstream HTTP parser).
 *
 * */

