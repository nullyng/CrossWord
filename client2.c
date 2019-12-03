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

typedef struct info{
	int selection;
	char *input;
};

int sock_id;

int main(int ac, char *av[])
{
	struct sockaddr_in servadd;
	struct hostent *hp;
	char message[BUFSIZ];
	int messlen;
	pthread_t t;
	int res;
	char str[BUFSIZ];
	char *temp[3];
	struct info *data;
	int i = 0;

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

	while(1){
		gets(str);
		if(strcmp(str,"break")==0)
			break;
		//write	
		write(sock_id,str,strlen(str)+1);
		//read
		read(sock_id, message, sizeof(message));

		printf("read : %s\n",message);
		temp[i] = strtok(message," ");
		while(temp[i] != NULL){
			printf("token %d: %s\n",i,temp[i]);
			temp[++i]=strtok(NULL," ");
		}

		sprintf(data->input,"%s %s",temp[1],temp[2]);
		data->selection = atoi(temp[0]);

		printf("struct char: %s , selection: %d",data->input,data->selection);

	}

	close(sock_id);
	
	return 0;
}
