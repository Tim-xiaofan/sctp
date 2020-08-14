/*sctp查询日期*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#define  MY_PORT_NUM  8080
#define  MAX_BUFFER  1024

int main(int argc, char *argv[])
{
	printf("Hello, SCTP\n");
	int connSock, in, i, flags;
	struct sockaddr_in servaddr;
	struct sctp_sndrcvinfo sndrcvinfo;//sctp_sndrcvinfo结构体，收到消息的时候会填写这个结构体，是一个输出参数
	struct sctp_event_subscribe events;
	char buffer[MAX_BUFFER + 1];
	/* Create an SCTP TCP-Style Socket */
	/*创建套接字,协议族AP_INET:IP，socket类型SOCKET:有序的、可靠的，IPPROTO_SCTP:指定为SCTP协议*/
	connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	/* Specify the peer endpoint to which we'll connect 设置端口、ip、协议*/
	bzero((void *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(MY_PORT_NUM);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	/* Connect to the server 建立连接*/
	connect(connSock, (struct sockaddr *)&servaddr, sizeof(servaddr));
	/* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
	memset((void *)&events, 0, sizeof(events));
	events.sctp_data_io_event = 1;
	setsockopt(connSock, SOL_SCTP, SCTP_EVENTS,
			   (const void *)&events, sizeof(events));
	/* Expect two messages from the peer */
	for (i = 0; i < 2; i++)
	{
		//接收消息
		in = sctp_recvmsg(connSock, (void *)buffer, sizeof(buffer),
						  (struct sockaddr *)NULL, 0,
						  &sndrcvinfo, &flags);
		/* Null terminate the incoming string终止字符串传入 */
		buffer[in] = 0;
		printf("Time:%s\n", buffer);
		/*if (sndrcvinfo.sinfo_stream == LOCALTIME_STREAM)
		{
			printf("(Local) %s\n", buffer);
		}
		else if (sndrcvinfo.sinfo_stream == GMT_STREAM)
		{
			printf("(GMT  ) %s\n", buffer);
		}*/
	}
	/* Close our socket and exit */
	close(connSock);
	return 0;
}
