#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#define oops(msg) { perror(msg); exit(1); }

int main(int ac, char *av[])
{
	struct sockaddr_in servadd;
	struct hostent *hp;
	int sock_id;
	char message[BUFSIZ];
	int messlen;

	if(ac != 3)
	{
		fprintf(stderr, "usage: %s ip port", av[0]);
		exit(1);
	}

	// get a socket
	sock_id = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_id == -1)
		oops("socket");

	// connet to server
	memset(&servadd, 0, sizeof(servadd));
	servadd.sin_family = AF_INET;
	servadd.sin_addr.s_addr = inet_addr(av[1]); // ip
	servadd.sin_port = htons(atoi(av[2])); // portnum

	if(connect(sock_id, (struct sockaddr *)&servadd, sizeof(servadd)) != 0)
		oops("connect");

	// transfer data from server, then hangup
	while(1)
	{	
		gets(message); // 메세지 입력

		if(strcmp(message, "quit") == 0) // quit일 때 break
			break;

		send(sock_id, message, sizeof(message), 0); // 서버로 메세지 보냄
	}

	close(sock_id);

	return 0;
}
