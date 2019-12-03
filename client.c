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
	int res;
	char str[BUFSIZ];

	if(ac != 3)
	{
		fprintf(stderr, "usage: %s ip port", av[0]);
		exit(1);
	}

	sock_id = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_id == -1)
		oops("socket");

	memset(&servadd, 0, sizeof(servadd));
	servadd.sin_family = AF_INET;
	servadd.sin_addr.s_addr = inet_addr(av[1]); // ip
	servadd.sin_port = htons(atoi(av[2])); // portnum

	if(connect(sock_id, (struct sockaddr *)&servadd, sizeof(servadd)) != 0)
		oops("connect");
	
	pthread_create(&t, NULL, receive, NULL);

	while(1)
	{
		gets(str);

		if(strcmp(str,"break")==0)
			break;	

		write(sock_id, str, strlen(str)+1);
		//read(sock_id, message, sizeof(message));
		//printf("read: %s\n", message);
	}
	
	pthread_join(t, NULL);
	close(sock_id);
	
	return 0;
}
	
void *receive()
{
	while(1)
	{
		char message[BUFSIZ];

		recv(sock_id, message, sizeof(message), 0);

		if(strcmp(message, "break") == 0)
			break;

		printf("receive: %s\n", message);
	}
}
