#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#define ECHO_DEF_PORT		7	/*侦听的默认端口*/
#define ECHO_BUF_SIZE		256	/*缓冲区大小*/

int main(int argc, char **argv)
{
	WSADATA wsa_data = {0};
	SOCKET hListenSocket = 0;	/*侦听句柄*/
	SOCKET hClientSocket = 0;	/*已连接句柄*/

	struct sockaddr_in serv_addr = {0};	/*socket本地地址,即服务器端地址*/
	struct sockaddr_in client_addr = {0};	/*socket远端地址,即客户端地址*/

	unsigned short port = ECHO_DEF_PORT;	/*默认端口*/
	int result = 0 ;	/*存储Winsock函数的返回值*/
	int addr_len = sizeof(struct sockaddr_in);

	char recv_buf[ECHO_BUF_SIZE];	/*缓冲区*/

	if (argc == 2)
	{
		port = atoi(argv[1]);
	}

	/************************************************************************/
	/* 初始化Winsock                                                                     */
	/************************************************************************/
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data))
	{
		printf("[Echo Server] WSAStartup error : %d !\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	
	/************************************************************************/
	/* 服务器创建一个套接字                                                                     */
	/************************************************************************/
	hListenSocket = socket(AF_INET, SOCK_STREAM, 0);	/*创建套接字*/
	
	/************************************************************************/
	/* 服务器把本地套接字地址绑定到该套接字                                                                     */
	/************************************************************************/
	/*socket套接字本地地址,即服务器端地址*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	/*INADDR_ANY表示通配符*/
	serv_addr.sin_port = htons(port);

	result = bind(hListenSocket, (const struct sockaddr *)&serv_addr, addr_len);	/*服务器把本地套接字地址(本地IP和本地端口)绑定到该套接字.*/
	if (SOCKET_ERROR == result)
	{
		printf("[Echo Server] bind error: %d !\n", WSAGetLastError());
		closesocket(hListenSocket);
		return EXIT_FAILURE;
	}

	/************************************************************************/
	/* 服务器把该套接字设置为等待状态,也叫监听状态.这时该套接字也被称为监听套接字                                                                     */
	/************************************************************************/
	if (SOCKET_ERROR == listen(hListenSocket, SOMAXCONN))
	{
		printf("[Echo Server] listen error: %d !\n", WSAGetLastError());
		closesocket(hListenSocket);
		return EXIT_FAILURE;

	}
	
	printf("[ Echo Server] is running... ...\n");

	/************************************************************************/
	/* 服务器开始接受客户端的连接请求                                                                     */
	/************************************************************************/
	while (1)
	{
		hClientSocket = accept(hListenSocket, (struct sockaddr *)&client_addr, &addr_len);	/*生成一个已连接套接字*/
		if (INVALID_SOCKET == hClientSocket)
		{
			printf("[Echo Server] accept error : %d !\n", WSAGetLastError());
			break;
		}
		/************************************************************************/
		/* 服务器使用已连接套接字来接受来自客户端的数据,或者发送数据到客户端                                                                     */
		/************************************************************************/

		result = recv(hClientSocket, recv_buf, ECHO_BUF_SIZE, 0);	/*接受数据(ECHO_BUF_SIZE为缓冲区大小,而不是要发送的数据的大小)*/
		if ( result > 0)
		{
			printf("[Echo Server] receives : \"%s\", from %s\r\n", recv_buf, inet_ntoa(client_addr.sin_addr));
			
			if (SOCKET_ERROR == send(hClientSocket, recv_buf, result, 0))/*将接受到的数据重新发送出去.(recv_buf为缓冲区大小,而不是要发送的数据的大小)*/
			{
				printf("[Echo Server] send error : %d !\n", WSAGetLastError());
				break;
			}
			
		}
	
		closesocket(hClientSocket);	/*释放套接字资源*/
	}
	
	closesocket(hListenSocket);
	WSACleanup();

	return EXIT_SUCCESS;
}