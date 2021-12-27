#ifndef _HTTP_PARSE_H_
#define _HTTP_PARSE_H_

#include "http_server.h"

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/

/*--------------------------------------------------*/
/* HTTP_PARSE 函数
*/
/*--------------------------------------------------*/
int httpParseReadLine(char *buf, char *pLine, int maxBufSum, int maxLineSum);
int splitWordBlack(char *line, char *word);
int parseHttpRequestMsgHead(char *line, HTTP_REQUEST_HEADER *pReqHead);
int getMethed(char *method, HTTP_REQUEST_HEADER *pReqHead);
int getVersion(char *version, HTTP_REQUEST_HEADER *pReqHead);
int parseHttpRequestMsgLine(char *line, HTTP_REQUEST_HEADER *pReqHead);
int parseHttpRequestData(char *buf, HTTP_REQUEST_DATA **ppHttpRequestData);

#endif // _HTTP_PARSE_H_