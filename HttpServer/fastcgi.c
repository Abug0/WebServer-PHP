#include "fastcgi.h"

/** 
 * 生成fcgi_header
 *
 * @param int type			记录类型
 * @param int requestId		请求ID
 * @param int contentLength	内容长度
 * @param int paddingLength	填充长度
 *
 * @return fcgi_header
 */
fcgi_header make_header(int type, int requestId, int contentLength, int paddingLength)
{
	fhr_t header;
	
	header.version = FCGI_VERSION_1;
	header.type = (unsigned char)type;
	header.requestIdB1 = (unsigned char)((requestId >> 8) & 0xff);
	header.requestIdB0 = (unsigned char)(requestId & 0xff);
	header.contentLengthB1 = (unsigned char)((contentLength >> 8) & 0xff);
	header.contentLengthB0 = (unsigned char)(contentLength & 0xff);
	header.paddingLength = (unsigned char)paddingLength;
	header.reserved = 0;
	
	return header;
}


/** 
 * 生成FCGI_BEGIN_REQUEST
 *
 * @param int role	服务器期望fcgi应用扮演的角色
 * @param int KeepConn	控制位，响应完成是否保持连接
 *
 * @return fcgi_begin_request_body
 */
fcgi_begin_request_body make_begin_request_body(int role, int keepConn)
{
	fbrb_t body;
	body.roleB1 = (unsigned char)((role >> 8) & 0xff);
	body.roleB0 = (unsigned char)(role & 0xff);
	body.flags = (unsigned char)keepConn;
	memset(body.reserved, 0, sizeof(body.reserved));
	
	return body;
}


/**
 * 发送FCGI_BEGIN_REQUEST
 *
 * @param int socket
 * @param int requestId
 *
 * @return int 发送成功返回0，失败返回-1
 */
int send_begin_request_record(int socket, int requestId)
{
	fbrr_t begin_record;
	fbrb_t body = make_begin_request_body(FCGI_RESPONDER, 0);
	fhr_t header = make_header(FCGI_BEGIN_REQUEST, requestId, sizeof(begin_record.body), 0);
	
	begin_record.header = header;
	begin_record.body = body;
	
	int ret = send(socket, (char*)&begin_record, sizeof(begin_record), 0);
	if (ret != sizeof(begin_record))
	{
		printf("Send FCGI_BEGIN_REQUEST ERROR!\n");
		return -1;
	}

	return 0;
}


/**
 * 发送参数记录
 *
 * @param int socket
 * @param int requestId
 * @param char* name	参数名
 * @param char* value	参数值
 *
 * @return int
 */
int send_params_record(int socket, int requestId, char *name, char *value)
{
	//printf("name:%s, value:%s.\n", name, value);
	
	/* 计算出contentLength和paddingLength */
	int name_length = strlen(name);
	int value_length = strlen(value);
	int content_length = name_length + value_length;
	content_length = (name_length < 128) ? ++content_length : content_length + 4;
	content_length = (value_length < 128) ? ++content_length : content_length + 4;
	int padding_length = (content_length % 8 == 0) ? 0 : (8 - content_length % 8);
	
	fhr_t header = make_header(FCGI_PARAMS, requestId, content_length, padding_length);
	
	char *buf = (char*)calloc(FCGI_HEADER_LEN + content_length + padding_length, sizeof(char));
	char *old = buf;
	memcpy(buf, &header, FCGI_HEADER_LEN);
	buf += FCGI_HEADER_LEN;
	
	/* 如果name_length<128，用一个字节保存，否则用四个字节 */
	if (name_length < 128)
		*buf++ = (unsigned char)name_length;
	else
	{
		*buf++ = (unsigned char)((name_length >> 24) | 0x80);
		*buf++ = (unsigned char)(name_length >> 16);
		*buf++ = (unsigned char)(name_length >> 8);
		*buf++ = (unsigned char)name_length;
	}
	
	/* 如果value_length<128，用一个字节保存，否则用四个字节 */
	if (value_length < 128)
		*buf++ = (unsigned char)value_length;
	else
	{
		*buf++ = (unsigned char)((value_length >> 24) | 0x80);
		*buf++ = (unsigned char)(value_length >> 16);
		*buf++ = (unsigned char)(value_length >> 8);
		*buf++ = (unsigned char)value_length;
	}
	
	/* 保存name和value */
	memcpy(buf, name, name_length);
	buf += name_length;
	memcpy(buf, value, value_length);
	
	int ret;
	ret = send(socket, old, FCGI_HEADER_LEN+content_length+padding_length, 0);
	fpr_t *pa = (fcgi_params_record*)old;
	//printf("nameLength:%d.\n", pa->nameLength);
	//printf("valueLength:%d.\n", pa->valueLength);
	//printf("data:%s.\n", pa->data);
	
	free(old);//释放空间，防止内存泄漏
	old = NULL;
	
	if (ret != FCGI_HEADER_LEN+content_length+padding_length)
	{
		printf("ret:%d.\n", ret);
		printf("SEND FCGI_PARAMS ERROR!\n");
		return -1;
	}
	
	return 0;
}


/**
 * 发送空的FCGI_PARAMS
 *
 * @params int socket
 * @params int requestId
 *
 * @return int
 */
int send_empty_params_record(int socket, int requestId)
{
	fhr_t header = make_header(FCGI_PARAMS, requestId, 0, 0);
	
	int ret = send(socket, (char*)&header, FCGI_HEADER_LEN, 0);
	if (ret < 0)
	{
		printf("SEND FCGI_EMPTY_PARAMS ERROR!\n");
		return -1;
	}
	
	return 0;
}


/**
 * 发送FCGI_STDIN
 *
 * @return int
 */
int send_stdin_record(int socket, int requestId, char *content)
{
	//printf("content:%s.\n", content);
	
	int content_length = strlen(content);
	int padding_length = (content_length % 8 == 0) ? 0 : (8 - content_length % 8);
	char padding_data[8] = {0};
	
	fhr_t header = make_header(FCGI_STDIN, requestId, content_length, padding_length);
	
	int ret;
	ret = send(socket, (char*)&header, FCGI_HEADER_LEN, 0);
	if (ret != FCGI_HEADER_LEN)
	{
		printf("SEND FCGI_STDIN ERROR!\n");
		return -1;
	}
	
	ret = send(socket, content, content_length, 0);
	if (ret != content_length)
	{
		printf("SEND FCGI_STDIN ERROR!\n");
		return -1;
	}
	
	ret = send(socket, padding_data, padding_length, 0);
	if (ret != padding_length)
	{
		printf("SEND FCGI_STDIN ERROR!\n");
		return -1;
	}
	
	return 0;
}


/**  
 * 发送空的FCGI_STDIN
 *
 * @params int socket
 * @params int requestId
 *
 * @return int
 */
int send_empty_stdin_record(int socket, int requestId)
{
	fhr_t header = make_header(FCGI_STDIN, requestId, 0, 0);
	
	int ret = send(socket, (char*)&header, FCGI_HEADER_LEN, 0);
	if (ret != FCGI_HEADER_LEN)
	{
		printf("SEND FCGI_STDIN ERROR!");
		return -1;
	}
	
	return 0;
}


/** 
 * 接收FCGI应用的响应
 *
 * @params int socket 与fcgi服务器相连接的socket
 *
 * @return int
 */
int recv_record(int socket, int requestId, char *buf, int len)
{
	/*
	fhr_t header;
	
	recv_header(socket, &header);
	
	printf("VERSION:%d.\n", header.version);
	printf("TYPE:%d.\n", header.type);
	printf("requestId:%d.\n", (int)(header.requestIdB1 << 8) + (int)header.requestIdB0);
	printf("CONTENTLENGTH:%d.\n", (int)(header.contentLengthB1 << 8) + (int)header.contentLengthB0);
	*/
	recv(socket, buf, len, 0);
	
	char *buf_tmp = (char*)calloc(len, sizeof(char));
	//char *str = (char*)calloc(2, sizeof(char));
	strcpy(buf_tmp, buf);
	int i = 0;
	while (i < strlen(buf))
	{
		if ((buf_tmp[i] == 13) && (buf_tmp[i+2] == 13))
		{
			strcpy(buf, buf_tmp+i+4);
			//printf("buf content:%s.\n", buf);
			break;
		}
		i++;
	}
	
	free(buf_tmp);
	buf_tmp = NULL;
	
	return 0;
}


/** 
 *
 */
int recv_header(int socket, fhr_t *header)
{
	int ret = recv(socket, (char*)header, 8, 0);
	if (ret < 0)
	{
		printf("RECV HEADER ERROR!\n");
		return -1;
	}
	
	return 0;
}