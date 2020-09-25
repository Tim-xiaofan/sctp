/*server.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
// #include "common.h"
#define MAX_BUFFER 256
#define MY_PORT_NUM 2222
#define LOCALTIME_STREAM 0
#define GMT_STREAM 1
int main()
{
	int listenSock, connSock, ret;
	struct sockaddr_in servaddr;
	struct sctp_initmsg initmsg;
	char buffer[MAX_BUFFER + 1];
	time_t currentTime;
	/* Create SCTP TCP-Style Socket */
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	/* Accept connections from any interface */
	bzero((void *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;//ip
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(MY_PORT_NUM);
	/* Bind to the wildcard address (all) and MY_PORT_NUM */
	ret = bind(listenSock,
			(struct sockaddr *)&servaddr, sizeof(servaddr));
	/* Specify that a maximum of 5 streams will be available per socket */
	memset( &initmsg, 0, sizeof(initmsg) );
	initmsg.sinit_num_ostreams = 5;
	initmsg.sinit_max_instreams = 5;
	initmsg.sinit_max_attempts = 4;
	ret = setsockopt( listenSock, IPPROTO_SCTP, SCTP_INITMSG,
			&initmsg, sizeof(initmsg) );
	/* Place the server socket into the listening state */
	listen(listenSock, 5);
	/* Server loop... */
	while (1)
	{
		/* Await a new client connection */
		connSock = accept(listenSock,
				(struct sockaddr *)NULL, (int *)NULL);
		/* New client socket has connected */
		/* Grab the current time */
		currentTime = time(NULL);
		/* Send local time on stream 0 (local time stream) */
		snprintf(buffer, MAX_BUFFER, "%s\n", ctime(&currentTime));
		printf("server:buffer = %s\n", buffer);
		/**/
		ret = sctp_sendmsg(connSock,
				(void *)buffer, (size_t)strlen(buffer),
				(struct sockaddr*)NULL, 0, 0, 0, LOCALTIME_STREAM, 0, 0);
		if(ret < 0)
		{
			perror("sctp_sendmsg");
			exit(errno);
		}
		/* Send GMT on stream 1 (GMT stream) */
		snprintf(buffer, MAX_BUFFER, "%s\n",
				asctime(gmtime(&currentTime)));
		ret = sctp_sendmsg(connSock,
				(void *)buffer, (size_t)strlen(buffer),
				(struct sockaddr*)NULL, 0, 0, 0, GMT_STREAM, 0, 0);
		if(ret < 0)
		{
			perror("sctp_sendmsg");
			exit(errno);
		}

		/* Close the client connection */
		close(connSock);
	}
	return 0;
}

