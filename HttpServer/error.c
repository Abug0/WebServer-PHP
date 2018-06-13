#include "error.h"


/** 
 * 根据错误码生成对应的响应体内容
 *
 * @param const int errorNum	错误码
 * @param char* responseBody	存放生成的响应体内容
 *
 * @return int
 */
int error(const int errorNum, char *responseBody)
{
	if(errorNum == 404)
		error_404(responseBody);
	
	
	return 0;
}


int error_404(char *responseBody)
{
	sprintf(responseBody, "%s", "<html><head></head><body>Hello<br>404_ERROR!</body></html>");
}