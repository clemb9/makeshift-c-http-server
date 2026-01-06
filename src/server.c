// server.c
#include"utils.h"

// Usage: ./server [ressource]:[path] ...
// Path relative from within the 

static volatile int running;

void sig_int(int signo)
{
	running = 0;
	Write(fileno(stdout),"^C\n", 3);
	return;
}	


int main(int argc, char **argv)
{
	int 		listenfd, connfd, optval;
	struct 		sockaddr_in servaddr, cliaddr;
	socklen_t	clilen;
	pid_t		child_pid;
	struct		rf_table lkp_table = {0};	
	
	running = 1;
	// prepping the ressource lookup table
	process_parameters(argc, argv, &lkp_table);
	
	// prepping server address
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	// prepping client address
	bzero(&cliaddr, sizeof(cliaddr));

	// creating the socket
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	// setting SO_REUSEADDR	
	optval = 1;
	Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval,	sizeof(optval));	
	
	// handling proc child signal
	Signal(SIGCHLD, sig_chld, 1);
	
	Signal(SIGINT, sig_int, 0);
	// cant figure out how to make it work for now, we can afford a little memory loss as a treat

	// the "bind and listen" part of "socket, bind and listen" 	
	Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	Listen(listenfd, LISTEN_QUEUE);
	
	while (running)
	{
		clilen = sizeof(cliaddr);

		if ( (connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen)) < 0)
		{
			if (errno == EINTR)
				continue;
			else
			{
				printf("accept error");
				err_print();
				exit(1);
			}		
		}
		
		printf("forking\n");	
		if ( (child_pid = Fork() == 0))
		{
			Close(listenfd); // close the duplicated listen
							 // socket on the child process 
							 // & decrement the listenfd counter
			serve(connfd, &lkp_table);
			Close(connfd);
			exit(0); // exit child process after serving
		}
		Close(connfd);
	}		
	
	printf("[.] Terminating server...\n");	
	Close(listenfd);
	free(lkp_table.table);
	return 0;
}
