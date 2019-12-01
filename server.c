#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define oops(msg) { perror(msg); exit(1); }

int main(int ac, char *av[])
{
	struct sockaddr_in serv_addr, clnt_addr;
	int serv_sock, sock_fd1, sock_fd2, value1, value2;
	char message1[BUFSIZ], message2[BUFSIZ];
	//char send1[] = "hi, i'm server. you're sock_fd1\n";
	//char send2[] = "hi, i'm server. you're sock_fd2\n";
	int s1, s2;

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
	memset(&serv_addr, 0, sizeof(serv_addr)); // serv_addr[0]부터 sizeof(serv_addr)만큼 0으로 지정	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(av[1]));

	if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
		oops("bind");

	// allow incoming calls with Qsize=1 on socket
	if(listen(serv_sock, 1) != 0)
		oops("listen");

	// accept
	sock_fd1 = accept(serv_sock, NULL, NULL); // accept은 blocking
	sock_fd2 = accept(serv_sock, NULL, NULL); // 2개가 들어올 때까지 기다림
	if(sock_fd1 == -1 || sock_fd2 == -1)
		oops("accept");
	printf("successfully connected!\n");

	/*
	s1 = send(sock_fd1, send1, strlen(send1), 0);
	s2 = send(sock_fd2, send2, strlen(send2), 0);
	if(s1 == -1 || s2 == -1)
		oops("send");
	*/
	
	while(1)
	{
		value1 = recv(sock_fd1, message1, sizeof(message1), MSG_DONTWAIT);
		value2 = recv(sock_fd2, message2, sizeof(message2), MSG_DONTWAIT);

		if(value1 != -1)
			printf("%s\n", message1);
		if(value2 != -1)
			printf("%s\n", message2);
	}
}
