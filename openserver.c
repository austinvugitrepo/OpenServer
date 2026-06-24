#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Simple IPv4 TCP server that repeats clients' messages.  
 * The server uses all local interfaces to listen on port 1234.
 * The server currently accepts any connection.
 */

static void childhandle(int signum)
{
	waitpid(WAIT_ANY, NULL, WNOHANG);
}

int
main(void)
{
	daemon(0, 0);
	int serv_fd, cli_fd;
	char mesg[2048] ; 
	ssize_t r;
	struct sockaddr_in address; 
	struct sigaction siga;
	pid_t pid;

	if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err(1, "socket call failed.");

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(1234);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(serv_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
		err(1, "bind call failed.");

	if (listen(serv_fd, 1) == -1)
		err(1, "listen call failed.");

	printf("listening on port 1234...\n");

	siga.sa_handler = childhandle;
	sigemptyset(&siga.sa_mask);

	siga.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &siga, NULL) == -1)
		err(1, "sigaction call failed.");

	for (;;) {
		if ((cli_fd = accept(serv_fd, NULL, NULL)) == -1)
			err(1, "accept call failed.");

		if ((pid = fork()) == -1)
			err(1, "fork call failed.");
		
		if (pid == 0) {	

		if (close(serv_fd) == -1)
			err(1, "close call failed.");

		for (;;) {
			if ((r = read(cli_fd, mesg, sizeof(mesg))) == -1)
				err(1, "read call failed.");

			if (r == 0)
				_exit(0);

			if (write(cli_fd, mesg, r) == -1)
				err(1, "write call failed.");

			if (write(cli_fd, "\n\n" , 2) == -1)
				err(1, "write call failed.");
		}


		}

		if (close(cli_fd) == -1)
			err(1, "close call failed.");
	}


	return 0;

}
