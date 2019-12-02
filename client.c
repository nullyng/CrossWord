#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define oops(msg) { perror(msg); exit(1); }

int sock_id;
void *receive();

int main(int ac, char *av[])
{
	struct sockaddr_in servadd;
	struct hostent *hp;
	char message[BUFSIZ];
	int messlen;
	pthread_t t;
	int result;

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

	pthread_create(&t, NULL, receive, NULL);
	// transfer data from server, then hangup
	while(1)
	{	
		gets(message); // 메세지 입력

		if(strcmp(message, "quit") == 0) // quit일 때 break
			break;

		send(sock_id, message, sizeof(message), 0); // 서버로 메세지 보냄
	}
	pthread_join(t, (void *)&result);

	close(sock_id);

	return 0;
}

void *receive()
{
	char msg[BUFSIZ];
	int value;

	while(1)
	{
		value = recv(sock_id, msg, strlen(msg), MSG_DONTWAIT);

		if(value != -1)
		{
			printf("receive: %s\n", msg);
		}
	}
}
