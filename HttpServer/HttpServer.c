#include "HttpServer.h" 
#include "awei_conf.h"

#pragma comment(lib, "ws2_32")

int main(void)
{
	/* 启动服务器，绑定监听地址和端口 */
	SOCKET socket_server = start_server();
	printf("START SERVER AT 127.0.0.1:%d.\n", get_server_port());
	
	while(1)
	{
		printf("---------------------------------------------\n");
		SOCKET socket_client = wait_request(socket_server);
		if(socket_client == -1)
		{
			printf("Connect Error!");
			continue;
		}
		
		/* 处理客户端请求 */
		server(socket_client);
		//Sleep(5000);
	}
	
	/* 关闭服务器 */
	close_server();
	return 0;
}


/** 
 * 启动服务器，绑定监听地址和端口，开始监听
 */
SOCKET start_server()
{
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2,2),&wsData)!=0)
    {
        printf("Failed to load Winsock.\n");
        exit(-1);
    }
	
	SOCKET socket_server = create_socket();
	bind_address(socket_server);
	listen(socket_server, get_max_connect_num());
	
	return socket_server;
}


/** 
 * 创建SOCKET
 */
SOCKET create_socket()
{
	struct sockaddr_in server_addr;
	
	SOCKET socket_server = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_server < 0)
	{
		printf("Socket Create Error!");
		exit(-1);
	}
	
	return socket_server;
}


/** 
 * 绑定地址和端口
 */
int bind_address(SOCKET sockfd)
{
	int server_port = get_server_port();
	
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	
	if( bind(sockfd, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR )
	{
		printf("Bind Error!");
		exit(-1);
	}
	
	return 0;
}


/** 
 * 等待客户请求
 */
SOCKET wait_request(SOCKET sockfd)
{
	printf("Waitting connect...\n");
	struct sockaddr_in client_addr;
	int addr_len = sizeof(client_addr);
	
	SOCKET socket_client = accept(sockfd, (SOCKADDR *)&client_addr, &addr_len);
	if(socket_client == SOCKET_ERROR)
		return -1;

	printf("Client address:%s.\n",inet_ntoa(client_addr.sin_addr));
	return socket_client;
}


/** 
 * 关闭服务器
 */
int close_server()
{
	WSACleanup();
}