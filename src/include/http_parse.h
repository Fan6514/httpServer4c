#ifndef _HTTP_PARSE_H_
#define _HTTP_PARSE_H_

#include "http_server.h"

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_LINE_LEN    1024

/*--------------------------------------------------*/
/* HTTP_PARSE 函数
*/
/*--------------------------------------------------*/

int httpParseReadLine(char *buf, char *pLine, int bufSum, int lineSum);
int parseHttpData(char *buf, HTTP_REQUEST_DATA *http_data);
int parseHttpRequestHead(char *head_buf, HTTP_REQUEST_DATA *http_data);

#endif // _HTTP_PARSE_H_