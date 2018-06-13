#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "awei_string.h"


/* 
 * 获取字符串中某个字符第一次出现时的索引
 *
 * @param const char* srcStr 源字符串
 * @param char chr 要查找的字符
 * 
 * @return int
 */
int get_char_index(const char *srcStr, char chr)
{
	if (!strchr(srcStr, chr))
		return 0;
	return (strchr(srcStr, chr)-srcStr)/sizeof(char);
}


/* 
 * 替换字符串中的子串
 */
int replace_str(char *srcStr, const char *from, const char *to)
{
	if(strlen(from) <= 0)
		return -EINVAL;
	
	int subStr_len = strlen(to);
	
	char *str;// = strstr(srcStr, from);
	
	while( (str = strstr(srcStr, from)) != NULL )	
	{
		int src_str_len = strlen(srcStr);
		int len_from = strlen(from);
		//printf("hhh\n");
		int pos_from = (str-srcStr)/sizeof(char);
		if(pos_from >= src_str_len)
			return -ERANGE;
		
		char *src_str_tmp = (char*)malloc((src_str_len-pos_from-1)*sizeof(char));
		get_sub_str(srcStr, src_str_tmp, pos_from+len_from, src_str_len-pos_from-len_from);
		srcStr[pos_from] = '\0';
		strcat(srcStr, to);
		strcat(srcStr, src_str_tmp);
		//printf("%s\n", srcStr);
	}

	return 0;
}

/* 
 * 删除字符串中指定位置开始的n个字符
 *
 * @param char* srcStr 子串所在的源字符串
 * @param int pos 删除字符的开始位置
 * @param int count 要删除的字符个数
 *
 * @return int 成功删除的字符数
 */
int del_sub_str(char *srcStr, int pos, int count)
{
	int src_str_len = strlen(srcStr);
	
	if(count < 0)
		return 0;
	
	if( pos<0 || pos>= src_str_len)
		return -EINVAL;

	char *src_str_tmp = (char*)malloc( src_str_len*sizeof(char) );
	if(get_sub_str(srcStr, src_str_tmp, pos+count, src_str_len-pos-count) < 0)
	{
		free(src_str_tmp);
		src_str_tmp = NULL;
		return -1;
	}

	srcStr[pos] = '\0';
	strcat(srcStr, src_str_tmp);
	
	free(src_str_tmp);
	src_str_tmp = NULL;
	
	return count;
}

/* 
 * 拷贝字符串中指定位置开始的n个字符
 *
 * @param const char* srcStr 被拷贝的源字符串
 * @param char* destStr 接收被拷贝的子串
 * @param int pos 拷贝的开始位置,起始偏移量为0
 * @param int count 要拷贝的字符数
 *
 * @return int 成功拷贝的字符数
 */
int get_sub_str(const char *srcStr, char *destStr, int pos, int count)
{
	int src_str_len = strlen(srcStr);
	
	/* 要复制的字符数小于0 */
	if( count<0 )
		return 0;
		
	/* pos越界或非法 */
	if ( (pos>=src_str_len) || (pos<0) )
		return -EINVAL;
	
	int i = pos;
	int j = 0;
	while( (j<count) && ((destStr[j++] = srcStr[i++]) != '\0') );

	/* 放置字符串结束符 */
	if(destStr[j] != '\0')
		destStr[j] = '\0';

	return strlen(destStr);
}


/* 
 * 在字符串的指定位置插入子串
 *
 * @param char* destStr 插入子串的目标字符串
 * @param const char* subStr 要插入的子串
 * @param int pos 子串的插入位置
 *
 * @return int 成功插入的字符数
 */
int insert_to_str(char *destStr, const char *subStr, int pos)
{
	int sub_str_len = strlen(subStr);
	int dest_str_len = strlen(destStr);
	
	/* pos非法,返回错误 */
	if( pos < 0 )
		return -EINVAL;
	
	if( pos >= dest_str_len)
	{
		strcat(destStr, subStr);
		return sub_str_len;
	}
	else
	{
		//为目标字符串扩展空间
		realloc(destStr, dest_str_len+sub_str_len*sizeof(char) );

		/* 将destStr中pos后的字符拷贝到dest_str_tmp进行临时保存 */
		char *dest_str_tmp = (char*)malloc( (dest_str_len-pos)*sizeof(char) );
		/* 临时保存失败，立即返回错误 */
		if( get_sub_str(destStr, dest_str_tmp, pos, dest_str_len-pos) < 0)
		{
			free(dest_str_tmp);
			dest_str_tmp = NULL;
			return -1;
		}
			
		/* 执行子串插入操作 */
		int i = pos;
		int j = 0;
		int cnt = 0;//记录插入的字符个数
		while( ((destStr[i++] = subStr[j++]) != '\0') && (cnt++ >= 0) );
		//printf("dest_str_tmp:%s.\n", dest_str_tmp);
		strcat(destStr, dest_str_tmp);
		
		free(dest_str_tmp);
		dest_str_tmp = NULL;
		
		return cnt;
	}

}