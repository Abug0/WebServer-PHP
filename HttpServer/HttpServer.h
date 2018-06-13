#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>

typedef int bool;
#define true 1
#define false 0


/* 启动服务器 */
SOCKET start_server();

/* 关闭服务器 */
int close_server();

/* 创建套接字 */
SOCKET create_socket();

/* 绑定监听地址 */
int bind_address(SOCKET sockfd);

/* 等待请求到达 */
SOCKET wait_request(SOCKET sockfd);

#endif
