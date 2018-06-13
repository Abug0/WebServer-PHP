#ifndef FASTCGI_H
#define FASTCGI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

//#define FCGI_HOST 127.0.0.1
//#define FCGI_POST 9000

//------------------------------------------------------
/* FCGI消息头 */
typedef struct{
	unsigned char version;			//FCGI协议版本
	unsigned char type;				//FCGI记录类型
	unsigned char requestIdB1;		
	unsigned char requestIdB0;		//FCGI记录所属的请求ID
	unsigned char contentLengthB1;	
	unsigned char contentLengthB0;	//内容长度
	unsigned char paddingLength;	//填充长度
	unsigned char reserved;			//保留字
}fcgi_header;

/* 消息头定长为8 */
#define FCGI_HEADER_LEN 8

/*  */
#define FCGI_VERSION_1 1

/**
 * FCGI记录的类型
 * 对应type字段的值
 */
#define FCGI_BEGIN_REQUEST	1
#define FCGI_ABORT_REQUEST	2
#define FCGI_END_REQUEST	3
#define FCGI_PARAMS			4
#define FCGI_STDIN			5
#define FCGI_STDOUT			6
#define FCGI_STDERR			7
#define FCGI_DATA			8
#define FCGI_GET_VALUES		9
#define FCGI_GET_VALUES_RESULT 10


/* FCGI_BEGIN_REQUEST */
typedef struct{
	unsigned char roleB1;
	unsigned char roleB0;		//服务器期望fcgi应用扮演的角色
	unsigned char flags;		//控制位，flags&KEEP_CONN,为0，响应请求后关闭线路，否则保持线路
	unsigned char reserved[5];
}fcgi_begin_request_body;

/**
 * fcgi应用可以扮演的角色
 * 对应role字段的值
 */
#define FCGI_RESPONDER	1
#define FCGI_AUTHORIZER	2
#define FCGI_FILTER		3


/* FCGI_END_REQUEST */
typedef struct{
	unsigned char appStatusB3;
	unsigned char appStatusB2;
	unsigned char appStatusB1;
	unsigned char appStatusB0;		//应用级别的状态码
	unsigned char protocolStatus;	//协议级别的状态码
	unsigned char reserved[3];
}fcgi_end_request_body;

#define FCGI_REQUEST_COMPLETE	0
#define FCGI_CANT_MPX_CONN		1
#define FCGI_OVERLOADED			2
#define FCGI_UNKNOWN_ROlE		3


/* 开始请求记录结构 */
typedef struct{
	fcgi_header header;
	fcgi_begin_request_body body;
}fcgi_begin_request_record;

/* 结束请求记录结构 */
typedef struct{
	fcgi_header header;
	fcgi_end_request_body body;
}fcgi_end_request_record;

/* 参数记录结构 */
typedef struct{
	fcgi_header header;
	unsigned char nameLength;
	unsigned char valueLength;
	unsigned char data[0];
}fcgi_params_record;


typedef fcgi_header fhr_t;
typedef fcgi_begin_request_body fbrb_t;
typedef fcgi_end_request_body ferb_t;
typedef fcgi_begin_request_record fbrr_t;
typedef fcgi_end_request_record ferr_t;
typedef fcgi_params_record fpr_t;


//----------开始函数声明--------------------------------------
/* 生成消息头 */
fcgi_header make_header(
		int type, 
		int requestId, 
		int contentLength, 
		int paddingLength
	);


/* 生成开始记录体 */
fcgi_begin_request_body make_begin_request_body(int role, int keepConn);

/* 发送开始记录 */
int send_begin_request_record(int socket, int requestId);

/* 发送参数记录 */
int send_params_record(
		int socket,
		int requestId,
		char *name,
		char *value
	);


/* 发送一个空的参数记录 */
int send_empty_params_record(int socket, int requestId);

/* 发送STDIN数据 */
int send_stdin_record(
		int socket,
		int requestId,
		char *content
	);

/* 发送一个空的STDIN数据 */
int send_empty_stdin_record(int socket, int requestId);

/* 接收FCGI记录 */
int recv_record(int socket, int requestId, char *buf, int len);
int recv_header(int socket, fhr_t *fhr);
int recv_body();

/*  */
int send_end_request_record();

#endif