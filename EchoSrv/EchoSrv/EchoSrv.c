#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#define ECHO_DEF_PORT		7	/*侦听的默认端口*/
#define ECHO_BUF_SIZE		256	/*缓冲区大小*/

int main(int argc, char **argv)
{
	WSADATA wsa_data = {0};
	SOCKET echo_soc = 0;	/*侦听句柄*/
	SOCKET acpt_soc = 0;	/*已连接句柄*/

	struct sockaddr_in serv_addr = {0};	/*socket本地地址,即服务器端地址*/
	struct sockaddr_in clnt_addr = {0};	/*socket远端地址,即客户端地址*/

	unsigned short port = ECHO_DEF_PORT;	/*默认端口*/
	int result = 0 ;	/*存储Winsock函数的返回值*/
	int addr_len = sizeof(struct sockaddr_in);

	char recv_buf[ECHO_BUF_SIZE];	/*缓冲区*/

	if (argc == 2)
	{
		port = atoi(argv[1]);
	}
	
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data))
	{
		printf("[Echo Server] WSAStartup error : %d !\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	echo_soc = socket(AF_INET, SOCK_STREAM, 0);

	/*socket套接字本地地址,即服务器端地址*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	/*INADDR_ANY表示通配符*/
	serv_addr.sin_port = htons(port);

	result = bind(echo_soc, (const struct sockaddr *)&serv_addr, addr_len);
	if (SOCKET_ERROR == result)
	{
		printf("[Echo Server] bind error: %d !\n", WSAGetLastError());
		closesocket(echo_soc);
		return EXIT_FAILURE;
	}

	if (SOCKET_ERROR == listen(echo_soc, SOMAXCONN))	/*设为监听套接字*/
	{
		printf("[Echo Server] listen error: %d !\n", WSAGetLastError());
		closesocket(echo_soc);
		return EXIT_FAILURE;

	}
	
	printf("[ Echo Server] is running... ...");

	while (1)
	{
		acpt_soc = accept(echo_soc, (struct sockaddr *)&clnt_addr, &addr_len);
		if (INVALID_SOCKET == acpt_soc)
		{
			printf("[Echo Server] accept error : %d !\n", WSAGetLastError());
			break;
		}

		result = recv(acpt_soc, recv_buf, ECHO_BUF_SIZE, 0);
		if ( result > 0)
		{
			recv_buf[result] = '\0';
			printf("[Echo Server] receives : \"%s\", from %s\r\n", recv_buf, inet_ntoa(clnt_addr.sin_addr));
			result = send(acpt_soc, recv_buf, result, 0);
		}
	
		closesocket(acpt_soc);
	}
	
	closesocket(echo_soc);
	WSACleanup();

	return EXIT_SUCCESS;
}