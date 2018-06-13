#include "handleRequest.h"
#include "parseRequest.h"


/** 
 * 服务器开始分析处理请求
 *
 * @param SOCKET socketClient 请求的发起者
 *
 * @return int
 */
int server(SOCKET socketClient)
{
	hhr_t hhr; // = (hhr_t*)calloc(sizeof(hhr_t));
	
	parse_request(socketClient, &hhr);
	
	/*
	printf("method:%s.\n", hhr.method);
	printf("uri:%s\n", hhr.uri);
	printf("version:%s\n", hhr.version);
	printf("name:%s\n", hhr.name);
	printf("filename:%s\n", hhr.filename);
	printf("args:%s\n", hhr.queryArgs);
	printf("contype:%s\n", hhr.contentType);
	printf("contlen:%s\n", hhr.contentLen);
	*/
	
	if (strstr(hhr.uri, "favicon.ico"))
		return 0;
	handle_request(socketClient, hhr);
	
	return 0;
}


/* 
 * 根据分析出的请求头进行处理
 *
 * 
 */
int handle_request(SOCKET socketClient, hhr_t hhr)
{
	int err = 404;
	if (!is_existed(hhr.filename))
	{
		error_not_found(socketClient, err);
		return 0;
	}

	if (is_dir(hhr.filename))
	{
		server_dir(socketClient, hhr);
		return 0;
	}

	char *delim = ".php";
	if (!strstr(hhr.name, delim))
		server_static(socketClient, hhr);
	else
		server_dynamic(socketClient, hhr);
	
	return 0;
}


//---------------------------------------------------
/** 
 * 请求中存在错误时
 *
 * @param SOCKET socketClient	客户端socket
 * @param int errno				错误号
 *
 * @return int
 */
int error_not_found(SOCKET socketClient, int errno)
{
	char *responseBody = (char*)calloc(1000, sizeof(char));
	
	sprintf(responseBody, "<html><head></head><body>Sorry!<br>404_NOT FOUND THE PAGE!</body></html>");
	//error_log();
	
	rhr_t rhr;
	strcpy(rhr.contentType, "text/html");
	itoa(strlen(responseBody), rhr.contentLen, 10);

	int ret = send_to_client(socketClient, &rhr, responseBody);
	
	free(responseBody);
	responseBody = NULL;
	
	if (ret < 0)
		return -1;
	return 0;
}


/** 
 * 客户端的请求对象是一个目录时，由server_dir进行响应
 */
int server_dir(SOCKET socketClient, hhr_t hhr)
{
	printf("server_dir\n");
	char *responseBody = (char*)calloc(10000, sizeof(char));
	get_html_with_dir(hhr.uri, responseBody);
	
	rhr_t rhr;
	strcpy(rhr.contentType, "text/html");
	itoa(strlen(responseBody), rhr.contentLen, 10);
	
	int ret = send_to_client(socketClient, &rhr, responseBody);
	
	free(responseBody);
	responseBody = NULL;

	if (ret < 0)
	{
		printf("SERVER_DIR:Send Error!\n");
		return -1;
	}
	return 0;
}


/**
 * 静态请求
 *
 * @param SOCKET socketClient
 * @param hhr_t hhr				本次请求的头部信息
 *
 * @return int
 */
int server_static(SOCKET socketClient, hhr_t hhr)
{
	int size = get_file_size(hhr.filename);
	
	char *responseBody = (char*)calloc(size, sizeof(char));
	get_file_content(hhr.filename, responseBody);
	
	rhr_t rhr;
	itoa(size, rhr.contentLen, 10); //Content-Length存入rhr
	
	/* 根据请求文件的类型获取对应的Content-Type */
	char *file_type = (char*)calloc(10, sizeof(char));
	get_file_type(hhr.name, file_type);
	get_content_type(file_type, rhr.contentType);
	
	//向客户端发送响应
	int ret = send_to_client(socketClient, &rhr, responseBody);

	free(file_type);
	free(responseBody);
	file_type = NULL;
	responseBody = NULL;
	
	if (ret < 0)
		return -1;
	return 0;
}


/** 
 * 动态请求
 *
 *
 */
int server_dynamic(SOCKET socketClient, hhr_t hhr)
{
	SOCKET fcgi_socket;
	if ((fcgi_socket = connect_to_fcgi(get_fcgi_host(), get_fcgi_port())) < 0)
	{
		printf("CONNECT FCGI ERROR!\n");
		return -1;
	}
	
	int requestId = fcgi_socket;
	send_to_fcgi(fcgi_socket, socketClient, &hhr);
	
	/* 接收fcgi头部信息 */
	fhr_t header;
	recv_header(fcgi_socket, &header);

	/* 接收fcgi响应体 */
	char *responseBody = (char*)calloc((int)(header.contentLengthB1 << 8) + (int)header.contentLengthB0, sizeof(char));
	recv_record(fcgi_socket, requestId, responseBody, (int)(header.contentLengthB1 << 8) + (int)header.contentLengthB0);
	closesocket(fcgi_socket);//关闭fcgi_socket
	
	rhr_t rhr;
	strcpy(rhr.contentType, "text/html");
	itoa(strlen(responseBody), rhr.contentLen, 10);

	int ret = send_to_client(socketClient, &rhr, responseBody);
	
	free(responseBody);
	responseBody = NULL;
	
	if (ret < 0)
		return -1;
	return 0;
}


//---------------------------------------------------------------
/** 
 *
 */
int send_to_client(SOCKET socketClient, rhr_t *rhr, const char *responseBody)
{
	/* 响应头信息 */
	strcpy(rhr->version, "HTTP/1.1");
	rhr->statusCode = 200;
	strcpy(rhr->msg, "success");
	strcpy(rhr->server, "NBS/1.0");
	strcpy(rhr->acceptRange, "bytes");
	strcpy(rhr->connection, "close");
	
	/* 生成响应头字符串 */
	char *responseHead = (char*)calloc(1024, sizeof(char));
	get_response_head(*rhr, responseHead);
	
	/* 发送响应 */
	int ret1 = send_response_head(socketClient, responseHead);
	int ret2 = send_response_body(socketClient, responseBody, atoi(rhr->contentLen));
	
	free(responseHead);
	responseHead = NULL;
	
	if ((ret1 < 0) || (ret2 < 0))
		return -1;
	return 0;
}


/** 
 * 发送响应
 *
 * @param const SOCKET socketClient	响应的发送目标
 * @param const char* responseHead	要发送的响应头
 * @param const char* responseBody	要发送的响应体
 *
 * @return int
 */
int send_response(const SOCKET socketClient, const char *responseHead, const char *responseBody)
{
	if ((send_response_head(socketClient, responseHead) < 0) || (send_response_body(socketClient, responseBody, strlen(responseBody)) < 0))
		return -1;
	return 0;
}


/** 
 * 发送响应头
 *
 * @param const SOCKET socketClient
 * @param const char* responseHead
 *
 * @return int
 */
int send_response_head(const SOCKET socketClient, const char *responseHead)
{
	if (send(socketClient, responseHead, strlen(responseHead), 0) != strlen(responseHead))
		return -1;
	return 0;
}


/** 
 * 发送响应体
 *
 * @param const SOCKET socketClient
 * @param const char* responseBody
 * @param int len 要发送的内容长度
 *
 * @return int
 */
int send_response_body(const SOCKET socketClient, const char *responseBody, int len)
{
	if (send(socketClient, responseBody, len, 0) < 0)
		return -1;
	return 0;
}


/** 
 * 获取响应头
 *
 * @param const rhr_t rhr			响应头信息
 * @param char* responseHead		存放响应头
 *
 * @return int
 */
int get_response_head(const rhr_t rhr, char *responseHead)
{
	
	
	sprintf(responseHead, "%s %d %s\r\n", rhr.version, rhr.statusCode, rhr.msg); //"HTTP/1.0 200 OK\r\n");
    sprintf(responseHead, "%sServer: %s\r\n", responseHead, rhr.server);
	sprintf(responseHead, "%sAccept-range: %s\r\n", responseHead, rhr.acceptRange);
    sprintf(responseHead, "%sConnection: %s\r\n", responseHead, rhr.connection);
	sprintf(responseHead, "%sContent-type: %s\r\n", responseHead, rhr.contentType);
	sprintf(responseHead, "%sContent-length: %s\r\n", responseHead, rhr.contentLen);	
	sprintf(responseHead, "%s\r\n", responseHead);

	return 0;
}


//---------------------------------------------------------------
/** 
 * 连接fcgi服务器
 */
SOCKET connect_to_fcgi(char *host, int port)
{
	SOCKET sockfd;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
	{  
		printf("Socket Error is %s\n", strerror(errno));  
		return -1;
	}  

	struct sockaddr_in fcgi_addr;
	fcgi_addr.sin_family = AF_INET;
	fcgi_addr.sin_port = htons(port);
	fcgi_addr.sin_addr.s_addr = inet_addr(host); 

	//客户端发出请求  
	if (connect(sockfd, (struct sockaddr *)(&fcgi_addr), sizeof(struct sockaddr)) == -1)
	{  
		printf("Connect failed:%s.\n", strerror(errno));
		return -1;
	}
	return sockfd;
}


/** 
 * 发送请求消息到fcgi
 */
int send_to_fcgi(SOCKET fcgiSocket, SOCKET socketClient, hhr_t *hhr)
{
	int requestId = fcgiSocket;
	char *params_name[] = {
		"REQUEST_URI",
		"SCRIPT_FILENAME",
		"SCRIPT_NAME",
		"REQUEST_METHOD",
		"QUERY_STRING",
		"CONTENT_TYPE",
		"CONTENT_LENGTH"
	};
	
	int params_offset[] = {
		(size_t) & (((hhr_t*)0)->uri),
		(size_t) & (((hhr_t*)0)->filename),
		(size_t) & (((hhr_t*)0)->name),
		(size_t) & (((hhr_t*)0)->method),
		(size_t) & (((hhr_t*)0)->queryArgs),
		(size_t) & (((hhr_t*)0)->contentType),
		(size_t) & (((hhr_t*)0)->contentLen)
	};
	
	/*  */
	send_begin_request_record(fcgiSocket, requestId);
	
	int i = 0;
	while (i < 7)
	{
		send_params_record(fcgiSocket, requestId, params_name[i], (char*)((long long)hhr+params_offset[i]));
		
		i++;
	}
	send_empty_params_record(fcgiSocket, requestId);
	 
	/* 获得请求体内容 */
	char *str = (char*)calloc(atoi(hhr->contentLen), sizeof(char));
	recv(socketClient, str, atoi(hhr->contentLen), 0);
	
	/* 发送请求体内容到fcgi */
	send_stdin_record(fcgiSocket, requestId, str);
	send_empty_stdin_record(fcgiSocket, requestId);
	
	return 0;
}


/*
//-------------------------------------------------------------------
int send_to_cgi(char *request)
{
	int sockfd;  
	char buffer[2014];  
	//struct sockaddr_in server_addr;  
	//struct hostent *host;  
	int nbytes;  

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
	{  
	  printf("Socket Error is %s\n", strerror(errno));  
	  exit(-1);  
	}  

	struct sockaddr_in server_addr;
	//bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

	//客户端发出请求  
	if (connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
	{  
	  printf("Connect failed:%s.\n", strerror(errno));
	  exit(-2);
	}

	//char *request = (char*)calloc(1024, sizeof(char));
	char *response = (char*)calloc(1024, sizeof(char));
	//sprintf(request, "GET / HTTP/1.1");
	
	send(sockfd, request, strlen(request), 0);
	//recv(sockfd, response, 1024, 0);
	recv(sockfd, response, 1024, 0);
	
	printf("response:%s.\n", response);
	
	fwrite(request, sizeof(char), 1024, stdin);
	fread(response, sizeof(char), 1024, stdout);
	
	char *cont = (char*)calloc(1024, sizeof(char));
	cont = getenv("CONTENT_TYPE");
	
	printf("cont:%s.\n", cont);
	printf("response:%s.\n", response);
	
	return 0;
}
*/