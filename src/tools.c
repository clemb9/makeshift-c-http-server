//tools.c
// More-under-the-hood related functions 
#include"utils.h"

// signal
Sigfunc *signal_custom(int signo, Sigfunc *func, int restart)
{
	// restart flag to precise whether we want it enabled or not
	struct sigaction act, oact; // action and old action

	act.sa_handler = func; // Passsing the function to the sigact struct handler parameter
	sigemptyset(&act.sa_mask); // Setting the block mask to an empty set
				   // This means no additionnal signals are being blocked
	act.sa_flags = 0; // Setting the other flags to 0
	if (signo == SIGALRM)
	{
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT; // Older systems equivalent to the lower 
#endif
	} else {
#ifdef SA_RESTART
		if (restart == 1)			// if the restart flag is set
			act.sa_flags |= SA_RESTART; // Specify SA_RESTART if it's defined
					    // (restart a system call if it's interrupted
					    // by our signal)
#endif
	}
	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);

	return (oact.sa_handler);
}




// writen
ssize_t writen(int fd, const void *vptr, size_t n)
{
	// write n bytes to file descriptor from buffer vptr
	size_t		nleft;
	ssize_t 	nwritten;
	const char	*ptr;

	ptr = vptr; // cast to char so you can increment it
	nleft = n;

	while (nleft >0)
	{
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) 
		{
			if (nwritten < 0 && errno == EINTR) // If interrupted by signal
							    // restart writing
				nwritten = 0;
			else
				return -1; // return error if n bytes were not written
		}
	
		nleft 	-= nwritten;
		ptr 	+= nwritten;
	}
	return n;
}


// readn

ssize_t readn(int fd, void *vptr, size_t n)
{
	// read n bytes from file descriptor fd
	ssize_t n_left;
	ssize_t n_read;
	char 	*ptr;
	
	ptr = vptr; // can't increment void pointer
	n_left = n;

	while (n_left > 0)
	{
		if ((n_read = read(fd, ptr, n_left)) < 0)
			{
			if (errno = EINTR)
				n_read = 0;
			else
				return -1;
			}
		else if (n_read == 0)
			break;
		n_left 	-= n_read;
		ptr	+= n_read;
	}
	return (n - n_left);
}

// baby function to quickly set a single flag on a socket with fcntl
void fcntl_setflag(int fd, int flag)
{
	int flags_set;
   	if	( (flags_set = fcntl(fd, F_GETFL, 0)) < 0)
	{
		printf("error getting flag");
		err_print();
		exit(1);	
	}
	
	flags_set |= flag;

	if (fcntl(fd, F_SETFL, flags_set) < 0)
	{
		printf("error setting flag");
		err_print();
		exit(1);	
	}
}

void fcntl_popflag(int fd, int flag)
{
	int flags_set;
   	if	( (flags_set = fcntl(fd, F_GETFL, 0)) < 0)
	{
		printf("error getting flag");
		err_print();
		exit(1);	
	}
	
	flags_set &= ~flag;

	if (fcntl(fd, F_SETFL, flags_set) < 0)
	{
		printf("error setting flag");
		err_print();
		exit(1);	
	}
}

size_t count_bytes(char *path_to_file)
{
	int 	fd, n, sum;
	char 	buf[MAX_BUF];
	fd = Open(path_to_file, O_RDONLY);
	
	while ( (n = Read(fd, buf, MAX_BUF)) > 0 )
		{
		sum+=n;
		}	
	Close(fd);
	return sum;
}

