#include "HttpServer.h"
#include "awei_string.h"
#include "parseRequest.h"


/** 
 * 读取一行
 *
 * @param const SOCKET socketClient 
 * @param char *buf 
 * @param int maxLine 每行的最大字符数
 *
 * @return int 读取进buf中的字符数
 */
int read_line(const SOCKET socketClient, char *buf, int maxLine)
{
	int i = 0;
	while ((recv(socketClient, buf+i, 1, 0) > 0) && (i < maxLine))
	{
		if (*(buf+i) == 10 )
		{
			*(buf+i-1) = 0; //清除读到的'\r\n'字符
			i = i - 2;
			break;
		}
		i++;
	}
	//printf("%s\n", requestLine);
	return i; //读取到的'\r\n'被丢弃,所以-2
}


/** 
 * 解析请求
 */
int parse_request(const SOCKET socketClient, hhr_t *hhr)
{
	char *requestLine = (char*)calloc(300, sizeof(char));
	get_request_line(socketClient, requestLine, 300);//读取出请求行
	
	/* 提取出method,uri,filename等信息 */
	get_method(requestLine, hhr->method);
	get_uri(requestLine, hhr->uri);
	get_version(requestLine, hhr->version);
	parse_uri(hhr->uri, hhr->filename, hhr->name, hhr->queryArgs);
	
	/* 如果是post请求，则提取请求体的相关信息 */
	if (strcmp(hhr->method, "POST") == 0)
		get_content_info(socketClient, hhr->contentType, hhr->contentLen);
		

	free(requestLine);
	requestLine = NULL;
	
	return 0;
}


/** 
 * 从请求中提取出请求行
 * 
 * @param const SOCKET socketClient 客户端连接
 * @param char *requestLine 存放提取出的请求行字符串
 * @param int maxLine 请求行可容纳的最大字符数
 *
 * @return int
 */
int get_request_line(const SOCKET socketClient, char *requestLine, int maxLine)
{
	read_line(socketClient, requestLine, maxLine);
	/*
	int i = 0;
	while (recv(socketClient, (requestLine+i), 1, 0) > 0)
	{
		if ( *(requestLine+i) == 10) //13=='\r',请求行已读取完
			break;
		i++;
	}
	*(requestLine+i-1) = 0; //清除读到的'\r'字符
	*(requestLine+i) = 0; //清除读到的'\n'字符
	//printf("%s\n", requestLine);
	*/
	return 0;
}


/** 
 * 解析uri
 * 从uri中提取出文件名、完整路径和查询参数
 *
 * @param const char *uri
 * @param char *name
 * @param char *filename
 * @param char *queryArgs
 *
 * @return int
 */
int parse_uri(const char *uri, char *filename, char *name, char *queryArgs)
{
	int index = get_char_index(uri, '?'); //定位到查询参数开始出现时的位置
	char *uri_tmp = (char*)calloc(256, sizeof(char));
	strcpy(uri_tmp, uri);

	/* 提取文件名和完整路径 */
	if (index)
	{
		uri_tmp[index] = 0;
		strcpy(queryArgs, uri+index+1); //提取uri中的参数信息
	}
		
	strcpy(name, uri_tmp);
	translate_uri_to_winpath(uri_tmp, filename);
	
	free(uri_tmp);
	uri_tmp = NULL;
	
	return 0;
}


//-----------------------------------------------------
/**
 * 解析出Http请求使用的方法
 *
 * @param const char* requestLine
 * @param char* method
 *
 * @return int
 */
int get_method(const char *requestLine, char *method)
{
	int i = 0;
	while(i<strlen(requestLine) && requestLine[i]!=' ')
	{
		method[i] = requestLine[i];
		i++;
	}

	method[i] = '\0';//字符串结束符，很重要，不加会乱码

	return 0;
}


/**
 * 解析出请求的资源路径和文件
 *
 * @param const char* requestLine
 * @param char* uri
 *
 * @return int
 */
int get_uri(const char *requestLine, char *uri)
{
	int i = 0;
	int j = 0;
	while(i<strlen(requestLine) && requestLine[i++]!=' ');

	while(i<strlen(requestLine) && requestLine[i]!=' ')
		uri[j++] = requestLine[i++];
	
	uri[j] = '\0';
	return 0;
}


/*
 * 解析出Http请求使用的协议版本号
 *
 * @param const char *requestLine
 * @param char* method
 *
 * @return int
 */
int get_version(const char *requestLine, char *version)
{
	int i = 0;
	int j = 0;
	int space_cnt = 0;
	while(i < strlen(requestLine))
	{
		if(requestLine[i] == ' ')
			space_cnt++;
		if(space_cnt >= 2)
			break;
		i++;
	}

	i++;
	while(requestLine[i]!='\r' && i<strlen(requestLine))
		version[j++] = requestLine[i++];

	version[j] = '\0';	
	return 0;
}


//------------------------------------------------------
int is_content_type(const char *str)
{
	char *tmp = (char*)calloc(15, sizeof(char));
	int i = 0;
	while (i++ < 12)
		tmp[i-1] = str[i-1];
	
	if (strcmp(tmp, "Content-Type") == 0)
		return 1;
	return 0;
}


int is_content_length(const char *str)
{
	char *tmp = (char*)calloc(15, sizeof(char));
	int i = 0;
	while (i++ < 14)
		tmp[i-1] = str[i-1];
	
	if (strcmp(tmp, "Content-Length") == 0)
		return 1;
	return 0;
}


int get_content_info(const SOCKET socketClient, char *type, char *length)
{
	char *info = (char*)calloc(50, sizeof(char));
	while (read_line(socketClient, info, 50) > 0)
	{
		if (is_content_type(info))
		{
			//printf("type:%s\n", info+14);
			strcpy(type, info+14);
		}
		else if (is_content_length(info))
		{
			//printf("len:%s\n", info+16);
			strcpy(length, info+16);
		}
			
	}
	return 0;
}