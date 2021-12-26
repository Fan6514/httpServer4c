#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "util.h"
#include "socket.h"
#include "thread_pool.h"

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_IPADDR_LEN      16
#define MAX_KEY_LEN         24
#define MAX_COOKIES_LEN     128
#define MAX_VALUE_LEN       512
#define MAX_URL_LEN         1024
#define MAX_HTTP_HEAD_LEN   1024
#define MAX_HTTP_BODY_LEN   10240

/*--------------------------------------------------*/
/* 数据结构 
*/
/*--------------------------------------------------*/
typedef enum { GET = 0, POST, PUT, DELETE, METHOD_NOT_SUPPORT }HTTP_METHOD;
typedef enum { HTTP_10 = 0, HTTP_11, VERSION_NOT_SUPPORT }HTTP_VERSION;
typedef enum { PARSE_REQUEST_LINE = 0, PARSE_REQUEST_HEAD, 
                PARSE_REQUEST_BODY, PARSE_COMPLATE, PARSE_UNDEFINED }PARSE_STATE;
typedef enum { UNCOMPLATE = 0, SEND_RESPONSE }RESPONSE_STATE;
typedef enum
{
    RETURN_STATE_OK = 200,
    RETURN_STATE_NOT_FUND = 404,
    RETURN_STATE_NOT_IMPLEMENTED = 501
} RETURN_STATE_CODE;


typedef struct http_state
{
    PARSE_STATE parse_state;        /* 解析报文的状态 */
    RESPONSE_STATE response_state;  /* 报文响应状态 */
}HTTP_STATE;

typedef struct http_request_header
{
    HTTP_METHOD method;             /* 请求方法 */
    HTTP_VERSION version;           /* http 协议版本 */
    char url[MAX_URL_LEN];          /* url */
    char host[MAX_VALUE_LEN];                 /* 主机地址 */
    BOOLEAN keep_alive;             /* 连接状态 */
    char cookie[MAX_COOKIES_LEN];   /* Cookie */
    int contentLen;                 /* 请求体长度 */
}HTTP_REQUEST_HEADER;

typedef struct http_request_data
{
    HTTP_STATE state;               /* http 请求处理状态 */
    HTTP_REQUEST_HEADER *header;    /* http 请求报文头 */
    char *body;                     /* http 请求报文体 */
}HTTP_REQUEST_DATA;

typedef struct http_response_header
{
    HTTP_VERSION version;           /* http 协议版本 */
    RETURN_STATE_CODE rtncode;      /* http 返回状态码 */
    char reason[MAX_KEY_LEN];       /* http 原因短语 */
    char contentType[MAX_KEY_LEN];  /* http 返回内容类型 */
}HTTP_RESPONSE_HEADER;


typedef struct http_response_data
{
    HTTP_STATE state;
    HTTP_RESPONSE_HEADER *header;   /* http 响应报文头 */
    char *body;                     /* http 响应报文体 */
}HTTP_RESPONSE_DATA;
/*--------------------------------------------------*/
/* HTTP 函数
*/
/*--------------------------------------------------*/
int httpServerRun(int port, int pollSize, int pollCoreSize);
int httpServerStartUp(int port, int pollSize, int pollCoreSize, ThreadPool **ppThread_pool, 
                        int *epoll_fd, SERVER_SOCKET *server_socket);
void httpServerEntry(void* arg);

#endif