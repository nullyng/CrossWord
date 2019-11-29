#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORTNUM 1342
#define HOSTLEN 256
#define oops(msg) { perror(msg); exit(1); }

int main(int ac, char *av[])
{
	struct sockaddr_in serv_addr;
	FILE *sock_fp;
	int serv_sock, sock_fd;
	char message[] = "Cross Word";

	if(ac != 2)
	{
		fprintf(stderr, "usage: %s port\n", av[0]);
		exit(1);
	}

	// ask kernel for a socket
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		oops("socket");

	// bind address to socket. Address is host, port
	memset(&serv_addr, 0, sizeof(serv_addr));	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(av[1]));

	if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
		oops("bind");

	// allow incoming calls with Qsize=1 on socket
	if(listen(serv_sock, 1) != 0)
		oops("listen");

	// main loop: accept(), write(), close()
	while(1)
	{
		sock_fd = accept(serv_sock, NULL, NULL);
		printf("got a call\n");
		if(sock_fd == -1)
			oops("accept");

		sock_fp = fdopen(sock_fd, "w");
		if(sock_fp == NULL)
			oops("fdopen");

		fprintf(sock_fp, "%s\n",  message);

		fclose(sock_fp);
	}
}
