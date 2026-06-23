#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Simple IPv4 TCP server that greets clients until interruption. 
 * The server uses all local interfaces to listen on port 1234.
 * The server currently accepts any connection.
 */

int
main(void)
{

	int serv_fd, cli_fd;
	const char *mesg = "Hello person connecting!\n"; 
	
	struct sockaddr_in address; 
	if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err(1, NULL);
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(1234);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(serv_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
		err(1, NULL);

	if (listen(serv_fd, 1) == -1)
		err(1, NULL);
	printf("listening on port 1234...\n");

	for (;;) { 
		if ((cli_fd = accept(serv_fd, NULL, NULL)) == -1)
			err(1, NULL);

		if (write(cli_fd, mesg, strlen(mesg)) == -1)
			err(1, NULL);
	
		if (close(cli_fd) == -1)
			err(1, NULL);
	}

	return 0;

}
