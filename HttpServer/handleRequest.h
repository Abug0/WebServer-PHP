#ifndef HANDLEREQUEST_H
#define HANDLEREQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#include "fastcgi.h"
#include "awei_conf.h"

//---------------------------------------------------

/* 请求头信息 */
typedef struct{
	char uri[256];			//uri
	char method[16];		//请求方法
	char version[16];		//版本
	char filename[256];		//文件名(包含完整路径)
	char name[256];			//文件名(不包含完整路径)
	char queryArgs[256];	//查询参数
	char contentType[256];		//请求体类型
	char contentLen[16];	//请求体长度
}http_header_t, hhr_t;


/* 响应头信息 */
typedef struct{
	char version[16];
	int statusCode;
	char msg[16];
	char server[32];
	char acceptRange[16];
	char connection[16];
	char contentType[32];
	char contentLen[16];	
}response_header_t, rhr_t;


//-------------------------------------------------------------

int server(SOCKET socketClient);

/* 请求存在问题 */
int serverError(SOCKET socketClient, int errno);

/* 请求对象为目录 */
int serveDir(SOCKET socketClient, hhr_t hhr);

/* 处理对静态页面的请求 */
int serverStatic(SOCKET socketClient, hhr_t hhr);

/* 处理对动态页面的请求 */
int serverDynamic(SOCKET socketClient, hhr_t hhr);

int handle_request(SOCKET socketClient, hhr_t hhr);

/* 获取请求体类型和长度 */
int get_contype(char *contype, int conLength);

int send_to_cgi(char *request);

SOCKET connect_to_fcgi(char *host, int port);
int send_to_fcgi(SOCKET fcgiSocket, SOCKET socketClient, hhr_t *hhr);

/* 向客户端发送响应 */
int send_to_client(SOCKET socketClient, rhr_t *rhr, const char *responseBody);

int get_response(const char *request, char *responseHead, char **responseBody);
int get_response_head(const rhr_t rhr, char *responseHead);
int set_response_head_arg(char *responseHead, const char *argName, const char *argValue);

int send_response(const SOCKET socketClient, const char *responseHead, const char *responseBody);
int send_response_head(const SOCKET socketClient, const char *responseHead);
int send_response_body(const SOCKET socketClient, const char *responseBody, int len);

#endif