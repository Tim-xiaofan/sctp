#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
// #include "common.h"
#define MAX_BUFFER 256
#define MY_PORT_NUM 2222
#define LOCALTIME_STREAM 256
#define GMT_STREAM 256
int main()
{
	int connSock, in, i, ret, flags;
	struct sockaddr_in servaddr;
	struct sctp_status status;
	struct sctp_sndrcvinfo sndrcvinfo;//sctp_sndrcvinfo结构体，收到消息的时候会填写这个结构体，是一个输出参数
	struct sctp_event_subscribe events;
	struct sctp_initmsg initmsg;//获取或设置某个SCTP套接字在发送INIT消息时所用的默认初始参数
	char buffer[MAX_BUFFER + 1];

	/* Create an SCTP TCP-Style Socket */
	/*创建套接字,协议族AP_INET:IP，socket类型SOCKET:有序的、可靠的，IPPROTO_SCTP:指定为SCTP协议*/
	connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

	/* Specify that a maximum of 5 streams will be available per socket */
	memset(&initmsg, 0, sizeof(initmsg));
	initmsg.sinit_num_ostreams = 5;//应用进程想要请求的外出sctp流的数目
	initmsg.sinit_max_instreams = 5;//应用进程准备允许的外来SCTP流的最大数目
	initmsg.sinit_max_attempts = 4;//SCTP协议栈应该重传多少次初始INIT消息才认为对端不可达
	//设置套接字
	ret = setsockopt(connSock, IPPROTO_SCTP, SCTP_INITMSG,
					 &initmsg, sizeof(initmsg));

	/* Specify the peer endpoint to which we'll connect */
	bzero((void *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(MY_PORT_NUM);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* Connect to the server 建立连接*/
	ret = connect(connSock, (struct sockaddr *)&servaddr, sizeof(servaddr));

	/* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
	memset((void *)&events, 0, sizeof(events));
	events.sctp_data_io_event = 1;
	ret = setsockopt(connSock, SOL_SCTP, SCTP_EVENTS,
					 (const void *)&events, sizeof(events));//事件注册

	/* Read and emit the status of the Socket (optional step) */
	in = sizeof(status);
	ret = getsockopt(connSock, SOL_SCTP, SCTP_STATUS,
					 (void *)&status, (socklen_t *)&in);

	printf("assoc id = %d\n", status.sstat_assoc_id);
	printf("state = %d\n", status.sstat_state);
	printf("instrms = %d\n", status.sstat_instrms);
	printf("outstrms = %d\n", status.sstat_outstrms);

	/* Expect two messages from the peer */

	for (i = 0; i < 2; i++)
	{
		//从connSock参数所指定的 SCTP 端点接收消息, buff是消息内容
		in = sctp_recvmsg(connSock, (void *)buffer, sizeof(buffer),
						  (struct sockaddr *)NULL, 0, &sndrcvinfo, &flags);

		if (in > 0)
		{
			buffer[in] = 0;
			if (sndrcvinfo.sinfo_stream == LOCALTIME_STREAM)
			{
				printf("(Local) %s\n", buffer);
			}
			else if (sndrcvinfo.sinfo_stream == GMT_STREAM)
			{
				printf("(GMT ) %s\n", buffer);
			}
		}
	}

	/* Close our socket and exit */
	close(connSock);

	return 0;
}