/** 
 * 对请求进行解析
 */

#ifndef PARSE_REQUEST_H
#define PARSE_REQUEST_H

#include <stdio.h>
#include <winsock2.h>

#include "handleRequest.h"

int read_line(const SOCKET socketClient, char *buf, int maxLine);

/* 解析请求,将请求头信息存放到hhr中 */
int parse_request(const SOCKET socketClient, hhr_t *hhr);

/* 提取出请求行 */
int get_request_line(const SOCKET socketClient, char *requestLine, int maxLine);

/* 解析uri,提取name,filename,args */
int parse_uri(const char *uri, char *filename, char *name, char *queryArgs);

/* 判断字符串中是否包含content-type信息 */
int is_content_type(const char *str);

/* 判断字符串中是否包含content-length信息 */
int is_content_length(const char *str);

/* 从给定的requestLine中提取出method,uri,version */
int get_method(const char *requestLine, char *method);
int get_uri(const char *requestLine, char *uri);
int get_version(const char *requestLine, char *version);

/*获取关于请求体的信息 */
int get_content_info(const SOCKET socketClient, char *type, char *length);

#endif