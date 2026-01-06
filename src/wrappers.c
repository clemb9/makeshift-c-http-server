// wrappers.c
#include"utils.h"

int Socket(int domain, int type, int protocol)
{
	int 	n;
	if ( (n = socket(domain, type, protocol)) < 0)
	{
		printf("Socket error:");
		err_print();
		exit(1);
	}

	return n;
}

// Internet presentation to numeric
int Inet_pton(int af, const char *restrict src, void *restrict dst)
{
	int	n;
	if ( (n = inet_pton(af, src, dst) <= 0) )
	{
		printf("inet_pton error with address: %s", src);
		err_print();
		exit(1);
	}

	return n;
}
// Internet numeric to presentation
const char *Inet_ntop(int af, const void *restrict src, char *dst, socklen_t size)
{
	const char *ptr;

	if ( (ptr = inet_ntop(af, src, dst, size)) == NULL )
	{
		printf("inet_ntop error");
		err_print();
		exit(1);
	}
	return ptr;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int 	n;
	if ( (n = connect(sockfd, addr, addrlen)) < 0)
	{
		printf("Connect error");
		err_print();
		exit(1);
	}

	return n;
}

ssize_t Read(int fd, void *ptr, size_t count)
{
	ssize_t	n;
	
	if ( (n = read(fd, ptr, count)) == -1)
	{	
		printf("Read error:");
		err_print();
		exit(1);
	}
	return n;
}

void Fputs(const char *restrict s, FILE *restrict stream)
{
	if (fputs(s, stream) == EOF)
	{
		printf("Fputs error");
		err_print();
		exit(1);
	}
}


void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
	{
		printf("Bind error");
		err_print();
		exit(1);
	}
}

void Listen(int fd, int backlog)
{
	if (listen(fd, backlog) < 0)
		{
		printf("Listen error");
		err_print();
		exit(1);
		}
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int	n;

	if ( (n = accept(fd, sa, salenptr)) < 0)
	{
		printf("Accept error");
		err_print();
		exit(1);
	}
	
	return n;
}

int Open(const char *path, int flags)
{
	int 	n;
	if ( (n = open(path, flags)) == -1 )
	{
		printf("Open error");
		err_print();
		exit(1);
	}
	
	return n;
}

void Close(int fd)
{
	if (close(fd) == -1)
	{
	printf("Close error");
	err_print();
	exit(1);
	}
}

void Write(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
	{
	printf("Write error");
	err_print();
	exit(1);
	}
}

// Tools wrappers
void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
	{
		printf("Writen error");
		err_print();
		exit(1);
	}

}

ssize_t Readn(int fd, void *ptr, size_t nbytes)
{
	ssize_t	n;

	if ( (n = readn(fd, ptr, nbytes)) < 0)
	{
		printf("readn error");
		err_print();
		exit(1);
	}
	return n;
}



pid_t Fork() 
{
	pid_t n;
	if ( (n = fork()) == -1)
	{
	printf("fork error");
	err_print();
	exit(1);	
	}	

	return n;	
}

Sigfunc *Signal(int signo, Sigfunc *func, int restart)
{
	Sigfunc *sigfunc;
	
	if ( (sigfunc = signal_custom(signo, func, restart)) == SIG_ERR)
	{
		printf("signal error");
		err_print();
	}	
	return(sigfunc);
}

char *Fgets(char *ptr, int n, FILE *stream)
{
	char *rptr;

	if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
		{
		printf("fgets error");
		err_print();
		exit(1);
		}

	return rptr;
}

int Select(int maxfdp1, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int n;
	if ( (n = select(maxfdp1, readfds, writefds, exceptfds, timeout)) < 0 )
	{
		printf("select error");
		err_print();
		exit(1);
	}
	return n;
}

void Shutdown(int fd, int how)
{
	if (shutdown(fd, how) < 0)
	{
		printf("shutdown error");
		err_print();
		exit(1);
	}
}
int Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen)
{
	int 	n;
	if ( (n = getsockopt(fd, level, optname, optval, optlen)) < 0)
		{
		printf("error fetching socket option");
		err_print();
		exit(1);
		}
	return n;
}


int Setsockopt(int fd, int level, int optname, void *optval, socklen_t optlen)
{
	int 	n;
	if ( (n = setsockopt(fd, level, optname, optval, optlen)) < 0)
		{
		printf("error setting socket option");
		err_print();
		exit(1);
		}
	return n;
}

