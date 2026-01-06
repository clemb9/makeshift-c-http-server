#ifndef header
#define header

#include<stdio.h>
#include<errno.h>
#include<fcntl.h>

#include<arpa/inet.h>
#include<signal.h>

#include<sys/stat.h>
#include<sys/select.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<sys/un.h>
#include<sys/param.h>
#include<netinet/in.h>
#include<netinet/tcp.h>

#include<string.h>
#include<syslog.h>
#include<poll.h>

#include<stdarg.h>
#include<unistd.h>
#include<stdlib.h>
#include<limits.h>
#include<time.h>

#include<fcntl.h>

#include<magic.h>

// Macros

#define MAX_LINE 		1024
#define	MAX_BUF			1024
#define MAX_REQ 		8912
#define MAX_URL 		2048
#define LISTEN_QUEUE 	5
#define TABLE_SIZE		8912

#define SERV_PORT 80
#define PFIX "../wwww/"

// file sizes
#define NOTFOUND 	426
#define BADREQ		419
// --- Enums
// Lower and upper case
enum {LOWER, UPPER};

// Requests type
enum  {INVALID, GET, POST, HEAD, PUT, DELETE, CONNECT, OPTIONS, TRACE, PATCH};

// Typedefs
typedef void Sigfunc(int); // facilitate the definition of the sigaction function

// Struct
struct rf_table // Table linking ressources names to file paths
{
	size_t 	n; // number of ressources
	size_t 	table_len;
	char 	*table;
};


struct params
{
	size_t 	language;	
	size_t 	accept;	
	char	host[256];
};
// Errors
void	err_print();

// Tools
ssize_t 	readn(int fd, void *vptr, size_t n);
//Sigfunc *signal(int signo, Sigfunc *func);
Sigfunc 	*signal_custom(int signo, Sigfunc *func, int restart);
ssize_t 	writen(int fd, const void *vptr, size_t n);
void		fcntl_setflag(int sockfd, int flag);
void		fcntl_popflag(int sockfd, int flag);
size_t		count_bytes(char *path_to_file);

// Network Wrappers
int		Socket(int domain, int type, int protocol);
int 	Inet_pton(int af, const char *restrict src, void *restrict dst);
const char * Inet_ntop(int af, const void *restrict src, char *dst, socklen_t size);
int 	Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void 	Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void 	Listen(int fd, int backlog);
int 	Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
int		Select(int maxfdp1, fd_set *readfds, fd_set *writefds, fd_set *except_fds, struct timeval *timeout);
void 	Shutdown(int fd, int how);

int 	Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen);
int		Setsockopt(int fd, int level, int optname, void *optval, socklen_t optlen);

// Io wrappers
int 	Open(const char *path, int flags);
void 	Close(int fd);
void 	Write(int fd, void *ptr, size_t nbytes);
char	*Fgets(char *ptr, int n, FILE *stream);
ssize_t Read(int fd, void *ptr, size_t count);
void 	Fputs(const char *restrict s, FILE *restrict stream);

// Unix wrappers
pid_t	Fork();
// Tools wrappers
ssize_t Readn(int fd, void *fptr, size_t n);
void 	Writen(int fd, void *ptr, size_t nbytes);
Sigfunc *Signal(int signo, Sigfunc *func, int restart);
// Signals
void 	sig_chld(int signo);

// Engine functions
char 	*convert_case(char *str, int n, int flag);
int		get_linesize(char *buffer);
ssize_t parse(char *request);
void 	serve(int sockfd, struct rf_table *lkp_table);
void	process_parameters(int argc, char **argv, struct rf_table *lkp_table);
void	get_content_type(char *ressource_path, char *dst);
int		get_ressource_path(char *name, char *dst, struct rf_table *lkp_table);	
size_t	process_headers(char *request, struct params *param_list);
void	print_help_exit();
// - Response functions
char 	*get_date_hdr(char *dst);
char 	*prep_response(char *dst, char *path_to_ressource);


// Server response
void 	process_get(int sockfd, char *ressource_path);
void 	resp_notfound(int sockfd);
void	resp_invalid(int sockfd);
void 	resp_get(int sockfd, char *ressource_path);
#endif
