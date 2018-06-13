/** 
 *
 */

#include "awei_conf.h"


/** 
 * 获取服务器端口
 */
int get_server_port()
{
	return SERVER_PORT;
}


/** 
 * 获取服务器根目录
 */
int get_root_dir(char *rootDir)
{
	strcpy(rootDir, ROOT_DIR);
	return 0;
}


/** 
 * 获取默认的index文件名
 */
int get_index_name(char *indexName)
{
	strcpy(indexName, INDEX_NAME);
	return 0;
}


/* 
 * 获取允许连接的最大客户端数量
 */
int get_max_connect_num()
{
	return MAX_CONNECT_NUM;
}


/** 
 * 获取fcgi_host
 */
char *get_fcgi_host()
{
	return FCGI_HOST;
}


/** 
 * 获取fcgi_port
 */
int get_fcgi_port()
{
	return FCGI_PORT;
}


/**  
 * 根据文件类型获取对应的Content-type
 * 
 * @param const char* fileType 文件类型
 * @param char* contType 存放文件类型对应的Content-type
 *
 * @return int 
 */
 /*
int get_cont_type(const char *fileType, char *contType)
{
	//if( strcmp(fileType, "html")==0 )
		//strcpy(contType, "text/html");
	char *type_file = (char*)calloc(10, sizeof(char));
	char *type_cont = (char*)calloc(50, sizeof(char));
	FILE *fp = fopen(CONT_TYPE_MAP_FILE, "r");
	
	if( !fp )
		contType = NULL;
	
	while( !feof(fp) )
	{
		char *str_line_fp = (char*)calloc(100, sizeof(char));
		fscanf(fp, "%s", str_line_fp);
		//fscanf(fp, "%s", str_line_fp);
		//printf("%s.\n", str_line_fp);
		//printf("%d.\n", get_chr_index(str_line_fp, ':'));
		get_sub_str(str_line_fp, type_file, 0, get_chr_index(str_line_fp, ':'));
		printf("type_file:%s.\n", type_file);
		//break;
		if( strcmp(type_file, fileType)==0 )
		{
			get_sub_str(str_line_fp, type_cont, get_chr_index(str_line_fp, ':')+1, strlen(str_line_fp)-1);
			printf("type_cont:%s.\n", type_cont);
			break;
		}
	}
	//printf();
	fclose(fp);
	return 0;
}
*/
