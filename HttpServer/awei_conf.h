/** 
 * 配置文件
 * 服务器的监听端口、根目录等配置参数
 */
#ifndef AWEI_CONF_H
#define AWEI_CONF_H

#include <stdio.h>
#include <string.h>

/* fcgi服务器地址和端口 */
#define FCGI_HOST "127.0.0.1"
#define FCGI_PORT 9000

/* 服务器端口 */
#define SERVER_PORT 8000

/* 网站根目录 */
#define ROOT_DIR "C:\\Users\\dell\\Desktop"

/* 默认门户 */
#define INDEX_NAME "index.html"

/* 最大客户端连接数量 */
#define MAX_CONNECT_NUM 10

/*  */
#define CONT_TYPE_MAP_FILE "content_type_map.txt"

int get_server_port();
int get_root_dir(char *rootDir);
int get_index_name(char *indexName);
int get_max_connect_num();

char *get_fcgi_host();
int get_fcgi_port();

#endif