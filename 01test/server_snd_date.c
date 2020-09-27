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
		/*size_t sctp_sendmsg(int s, const void *msg, size_t len, const struct sockaddr *to, socklen_t tolen, 
		 * uint32_t ppid, uint32_t flags, uint16_t stream_no, uint32_t timetolive, uint32_t context);
		 * sctp_sendmsg() 函数在发送来自 SCTP 端点的消息时启用高级 SCTP 功能。
		 *
		 * s
		 * 此值指定发送消息的 SCTP 端点。
		 *
		 * msg
		 * 此值包含 sctp_sendmsg() 函数所发送的消息。
		 *
		 * len
		 * 此值为消息的长度，以字节为单位。
		 *
		 * to
		 * 此值为消息的目标地址。配合flags参数默认不使用，目的地址为关联的主要的目标地址
		 *
		 * tolen
		 * 此值为目标地址的长度。
		 *
		 * ppid
		 * 此值为应用程序指定的有效负荷协议标识符。
		 *
		 * stream_no
		 * 此值为此消息的目标流。
		 *
		 * timetolive
		 * 此值为消息未能成功发送到对等方的情况下消息过期之前可以等待的时间段，以毫秒为单位。
		 *
		 * context
		 * 如果在发送消息时出现错误，则返回此值。
		 *
		 * flags
		 * 此值在将逻辑运算 OR 以按位形式应用于以下零个或多个标志位时形成：
		 *
		 * MSG_UNORDERED
		 * 设置此标志之后，sctp_sendmsg() 函数将无序传送消息。
		 *
		 * MSG_ADDR_OVER
		 * 设置此标志之后，sctp_sendmsg() 函数将使用 to 参数中的地址，而不使用关联的主要目标地址。此标志仅用于一对多风格 SCTP 套接字。
		 *
		 * MSG_ABORT
		 * 设置此标志之后，指定的关联将异常中止，同时向其对等方发送 ABORT 信号。此标志仅用于一对多风格 SCTP 套接字。
		 *
		 * MSG_EOF
		 * 设置此标志之后，指定的关联将进入正常关机状态。此标志仅用于一对多风格 SCTP 套接字。
		 *
		 * MSG_PR_SCTP
		 * 设置此标志之后，如果消息传输未在 timetolive 参数所指定的时间段内成功完成，则消息将过期。
		 *
		 * sctp_sendmsg() 函数将返回其发送的字节数。sctp_sendmsg() 函数将在出现错误时返回 -1。*/
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

