#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <errno.h>
#define MAX_BUFFER 256
#define MY_PORT_NUM 2222
#define LOCALTIME_STREAM 0
#define GMT_STREAM 1
int main()
{
	int connSock, in, i, flags, ret;
	struct sockaddr_in servaddr;
	struct sctp_sndrcvinfo sndrcvinfo;
	struct sctp_event_subscribe events;
	char buffer[MAX_BUFFER + 1];
	/* Create an SCTP TCP-Style Socket */
	connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if(connSock == -1)
	{
		printf("socket err\n");
		exit(-1);
	}
	/* Specify the peer endpoint to which we'll connect */
	bzero((void *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(MY_PORT_NUM);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	/* Connect to the server */
	ret = connect(connSock, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(ret == -1)
	{
		perror("connect");
		exit(errno);
	}
	printf("connect success to %s, port %d, sock %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port), connSock);
	/* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
	memset((void *)&events, 0, sizeof(events));
	events.sctp_data_io_event = 1;
	setsockopt(connSock, SOL_SCTP, SCTP_EVENTS,
			   (const void *)&events, sizeof(events));
	/* Expect two messages from the peer */
	for (i = 0; i < 2; i++)
	{
		in = sctp_recvmsg(connSock, (void *)buffer, sizeof(buffer),
						  (struct sockaddr *)NULL, 0,
						  &sndrcvinfo, &flags);
		/* Null terminate the incoming string */
		if(in < 0)
		{
			perror("sctp_recvmsg");
			exit(errno);
		}
		printf ("\n-----data-----%ld\n", strlen(buffer));
		buffer[in] = 0;
		if (sndrcvinfo.sinfo_stream == LOCALTIME_STREAM)
		{
			printf("(Local) %s\n", buffer);
		}
		else if (sndrcvinfo.sinfo_stream == GMT_STREAM)
		{
			printf("(GMT  ) %s\n", buffer);
		}
	}
	/* Close our socket and exit */
	close(connSock);
	return 0;
}
