#include "HttpServer.h"

#include <stdio.h>
#include <io.h>
#include <dos.h>
#include <windows.h>
#include <regex.h>
#include <errno.h>


/** 
 * 将请求行中的uri转化为windows下的路径
 *
 * @param const char* uri 请求行中的uri
 * @param char* path 存放由uri转化的Windows路径
 *
 * @return int
 */
int translate_uri_to_winpath(const char *uri, char *path)
{
	strcpy(path, "C:\\\\Users\\\\dell\\\\Desktop");
	if ( strcmp(uri, "/")==0 )
	{
		return 0;
	}

	char *uri_tmp = (char*)calloc(strlen(uri)+99, sizeof(char));
	strcpy(uri_tmp, uri);
	replace_str(uri_tmp, "/", "\\\\");
	strcat(path, uri_tmp);
	//printf("hhh\n");
	free(uri_tmp);
	uri_tmp = NULL;
	
	return 0;
}


/** 
 * 判断给定路径是否存在
 *
 * @param const char* winpath
 *
 * @return bool
 */
bool is_existed(const char *winpath)
{
	struct _finddata_t file;

	printf("winpath ex:%s.\n", winpath);
	if( _findfirst(winpath, &file) < 0 )
		return false;
	
	return true;
}


/** 
 * 判断路径指向的是否是目录
 *
 * @param const char* winpath 要判断的目录
 *
 * @return bool 如果是目录，返回true，否则false
 */
bool is_dir(const char *winpath)
{
	struct _finddata_t file;

	if( _findfirst(winpath, &file) < 0 )
		return false;

	if( file.attrib & _A_SUBDIR )
		return true;
	
	return false;
}


/** 
 * 根据路径及文件名获取文件类型
 *
 * @param const char* fileName 文件路径
 * @param char* type 存放文件类型
 *
 * @return int 
 */
int get_file_type(const char *fileName, char *type)
{
	//检查是否存在
	if( !is_existed(fileName) )
		return -EINVAL;
		
	//检查是否是目录
	if( is_dir(fileName) )
		return -EINVAL;
		
	int len_path = strlen(fileName);
	int i = len_path-1;
	int j = 0;
	char *type_tmp = (char*)calloc(50, sizeof(char));
	while( (i>=0) && ((type_tmp[j++]=fileName[i--]) != '.') );
		//printf("%s.\n", type_tmp);
	i = strlen(type_tmp)-2;
	j = 0;
	
	while( (i>=0) && (type[j++] = type_tmp[i--]) );

	//printf("%s.\n", type);
	type[++j] = '\0';
	
	free(type_tmp);
	type_tmp = NULL;

	return 0;
}


/** 
 * 获取文件大小
 *
 * @param consr char* winpath 目标文件
 *
 * @return int 文件大小，单位为字节
 */
int get_file_size(const char* winpath)
{
	FILE *fp = fopen(winpath, "r");
	if( !fp )
		return -1;
	
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	//printf();
	return size;
}


/**
 * 遍历目录，列出目录下所有文件和子目录
 *
 * @param const char* dir 要读取的目录
 * @param char* buf
 *
 * @return int 
 */
int read_dir(const char *uri, const char *dir, char *buf)
{
	buf[0] = '\0';

	struct  _finddata_t file;
	long lfDir;
	char *dir_tmp = (char*)calloc(strlen(dir)+5, sizeof(char));
	char *uri_tmp = (char*)calloc(strlen(uri), sizeof(char));
	
	sprintf(dir_tmp, "%s\\*.*", dir);
	if( strcmp(uri, "/")==0 )
		sprintf(uri_tmp, "");
	else
		sprintf(uri_tmp, "%s", uri);

	if((lfDir = _findfirst(dir_tmp, &file))==-1)
		file_not_found();	
	else
	{
		/* 跳过"."和".."两个目录 */
		_findnext(lfDir, &file);
		_findnext(lfDir, &file);
		
		//printf("file list:\n");
		do
		{
			//printf("buf:%s\n", buf);
			if( file.attrib & _A_SUBDIR )
				sprintf(buf, "%s<a href='%s/%s'>%s/</a><br>", buf, uri_tmp, file.name, file.name);
			else	  
				sprintf(buf, "%s<a href='%s/%s'>%s</a><br>", buf, uri_tmp, file.name, file.name);
		}while( _findnext(lfDir, &file) == 0 );
	}

	free(dir_tmp);
	dir_tmp = NULL;
	free(uri_tmp);
	uri_tmp = NULL;
	
	sprintf(buf, "%s\0", buf);
	_findclose(lfDir);
	
	return 0;
}


/** 
 * 读取文件内容
 *
 * @param const char* winpath 要读取的文件
 * @param void* buf 存储文件内容
 */
int read_file(const char* winpath, void **buf)
{
	int size = get_file_size(winpath);
	*buf = realloc(*buf, size);//调整buf内存大小
	memset(*buf, 0, size);
	
	FILE *fp = fopen(winpath, "rb+");
	FILE *fp1 = fopen("C:\\Users\\dell\\Desktop\\tmp.txt", "wb+");
	if( !fp )
		return -1;

	fread(*buf, sizeof(char), size, fp);
	fwrite(*buf, sizeof(char), size, fp1);
	fclose(fp);
	fclose(fp1);

	return 0;
}


/** 
 * 没有找到资源，发送404
 */
int file_not_found()
{
	printf("404...Not Fonud!");
	return 0;
}
/*
int main()
{
	char *dir = "C:\\Users\\dell\\Desktop\\HttpServer\\*.*";
	read_dir(dir);
	return 0;
}
*/