#include "handleResource.h"
#include "awei_conf.h"
#include "awei_string.h"

/** 
 * 将请求行中的uri转化为windows下的路径
 *
 * @param const char* uri	请求行中的uri
 * @param char* winPath		存放由uri转化的Windows路径
 *
 * @return int
 */
int translate_uri_to_winpath(const char *uri, char *winPath)
{
	//printf("tt\n");
	char *root_dir = (char*)calloc(50, sizeof(char));
	get_root_dir(root_dir);
	strcpy(winPath, root_dir);
	replace_str(winPath, "/", "\\\\");
	
	if ( strcmp(uri, "/")==0 )
	{
		return 0;
	}

	char *uri_tmp = (char*)calloc(strlen(uri)+60, sizeof(char));
	strcpy(uri_tmp, uri);
	replace_str(uri_tmp, "/", "\\");
	strcat(winPath, uri_tmp);
	//printf("winPath:%s.\n", winPath);
	
	free(uri_tmp);
	uri_tmp = NULL;
	
	return 0;
}


/** 
 * 判断给定路径是否存在
 *
 * @param const char* winPath
 *
 * @return bool
 */
bool is_existed(const char *winPath)
{
	struct _finddata_t file;

	//printf("winpath ex:%s.\n", winPath);
	if( _findfirst(winPath, &file) < 0 )
		return false;
	
	return true;
}


/** 
 * 判断路径指向的是否是目录
 *
 * @param const char* winPath 要判断的目录
 *
 * @return bool 如果是目录，返回true，否则false
 */
bool is_dir(const char *winPath)
{
	struct _finddata_t file;

	if( _findfirst(winPath, &file) < 0 )
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
int get_file_type(const char *fileName, char *fileType)
{
	//检查是否是目录
	if( is_dir(fileName) )
		return -EINVAL;
		
	int len_path = strlen(fileName);
	int i = len_path-1;
	int j = 0;
	char *type_tmp = (char*)calloc(50, sizeof(char));
	while( (i>=0) && ((type_tmp[j++]=fileName[i--]) != '.') );

	i = strlen(type_tmp)-2;
	j = 0;
	while( (i>=0) && (fileType[j++] = type_tmp[i--]) );

	//printf("%s.\n", type);
	fileType[++j] = '\0';
	
	free(type_tmp);
	type_tmp = NULL;

	return 0;
}


/**  
 * 根据文件类型获取对应的Content-type
 * 
 * @param const char* fileType	文件类型
 * @param char* contentType		存放文件类型对应的Content-type
 *
 * @return int 
 */
int get_content_type(const char *fileType, char *contentType)
{
	char *tmp_file_type = (char*)calloc(10, sizeof(char));
	char *tmp_content_type = (char*)calloc(50, sizeof(char));
	FILE *fp = fopen(CONT_TYPE_MAP_FILE, "r");
	
	if( !fp )
		contentType = NULL;
	
	while( !feof(fp) )
	{
		char *str_line_fp = (char*)calloc(100, sizeof(char));
		fscanf(fp, "%s", str_line_fp);
		get_sub_str(str_line_fp, tmp_file_type, 0, get_char_index(str_line_fp, ':'));
		
		if( strcmp(tmp_file_type, fileType)==0 )
		{
			get_sub_str(str_line_fp, contentType, get_char_index(str_line_fp, ':')+1, strlen(str_line_fp)-1);
			break;
		}
	}
	
	if(feof(fp))
		strcpy(contentType, "text/html");

	fclose(fp);
	return 0;
}


/** 
 * 获取文件大小
 *
 * @param consr char* winPath 目标文件
 *
 * @return int 文件大小，单位为字节
 */
int get_file_size(const char* winPath)
{
	FILE *fp = fopen(winPath, "r");
	if( !fp )
		return 0;
	
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	return size;
}


/** 
 * 读取文件内容
 *
 * @param const char* fileName	要读取的文件
 * @param void* content				存储文件内容
 */
int get_file_content(const char *fileName, char *content)
{
	/*
		*content = realloc(*content, size);//调整buf内存大小
		memset(*content, 0, size);
		printf("size:%d.\n", size);
	*/
	
	int size = get_file_size(fileName);
	FILE *fp = fopen(fileName, "rb+");
	//FILE *fp1 = fopen("C:\\Users\\dell\\Desktop\\tmp.txt", "wb+");
	if( !fp )
		return -1;

	
	fread(content, sizeof(char), size, fp);
	//printf("size:%d.\n", size);
	//fwrite(*content, sizeof(char), size, fp1);
	fclose(fp);
	//fclose(fp1);

	return 0;
}


/**  
 * 找到默认的index文件
 */
int get_index(char *winPath)
{
	char *index_name = (char*)calloc(20, sizeof(char));
	char *index_path = (char*)calloc(strlen(winPath)+25, sizeof(char));
	
	get_index_name(index_name);
	strcpy(index_path, winPath);
	strcat(index_path, "\\");
	strcat(index_path, index_name);
	
	if(!is_existed(index_path))
		return 0;
	
	strcpy(winPath, index_path);
	return 1;
}


/** 
 * 根据目录生成对应的html
 */
int get_html_with_dir(const char *uri, char *html)
{
	struct  _finddata_t file;
	long lfDir;
	char *dir = (char*)calloc(256, sizeof(char));
	char *uri_tmp = (char*)calloc(256, sizeof(char));
	
	translate_uri_to_winpath(uri, dir);
	sprintf(dir, "%s\\*.*", dir);
	
	if( strcmp(uri, "/")==0 )
		sprintf(uri_tmp, "");
	else
		sprintf(uri_tmp, "%s", uri);
	
	if((lfDir = _findfirst(dir, &file))==-1)
		return 0;	
	else
	{
		/* 跳过"."和".."两个目录 */
		_findnext(lfDir, &file);
		_findnext(lfDir, &file);
		
		do
		{
			if( file.attrib & _A_SUBDIR )
				sprintf(html, "%s<a href='%s/%s'>%s/</a><br>", html, uri_tmp, file.name, file.name);
			else	  
				sprintf(html, "%s<a href='%s/%s'>%s</a><br>", html, uri_tmp, file.name, file.name);
		}while( _findnext(lfDir, &file) == 0 );
	}
	
	free(dir);
	free(uri_tmp);
	dir = NULL;
	uri_tmp = NULL;
	
	return 0;
}