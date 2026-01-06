//signals.c
#include"utils.h"

void sig_chld(int signo)
{
	pid_t	pid;
	int 	stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) // Wait for child to terminate
	{		  			// and capture its PID
	
	if (WIFSIGNALED(stat))
	{
		if (WTERMSIG(stat) == SIGSEGV)
			{
			printf("segfault on child process: ");
			}
	}
	printf("child %d terminated\n", pid); // Not asynchronous safe! replace this in the long run	
					      // WNOHANG: do not block if there are no terminated 
					      // children
	}
	return;	
}

