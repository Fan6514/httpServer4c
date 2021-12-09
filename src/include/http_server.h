#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "util.h"

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_IPADDR_LEN      15
#define MAX_COOKIES_LEN     125
#define MAX_URL_LEN         1024
#define MAX_HTTP_HEAD_LEN   1024
#define MAX_HTTP_BODY_LEN   10240

/*--------------------------------------------------*/
/* 数据结构 
*/
/*--------------------------------------------------*/
typedef enum { GET = 0, POST, PUT, DELETE, METHOD_NOT_SUPPORT }HTTP_METHOD;
typedef enum { HTTP_10 = 0, HTTP_11, VERSION_NOT_SUPPORT }HTTP_VERSION;
typedef enum { KEEP_ALIVE = 0 }HTTP_CONNECTION;
typedef enum { PARSE_REQUEST_LINE = 0, PARSE_REQUEST_HEAD, PARSE_REQUEST_BODY }PARSE_STATE;

typedef struct 
{
    char *ip;
    int port;
}HOST_ADDR;

typedef struct http_state
{
    PARSE_STATE parse_state;        /* 解析报文的状态 */
}HTTP_STATE;

typedef struct http_request_header
{
    HTTP_METHOD method;             /* 请求方法 */
    HTTP_VERSION version;           /* http 协议版本 */
    char url[MAX_URL_LEN];          /* url */
    HOST_ADDR host;                 /* 主机地址 */
    boolean keep_alive;             /* 连接状态 */
    char cookie[MAX_COOKIES_LEN];   /* Cookie */
    int contentLen;                 /* 请求体长度 */
}HTTP_REQUEST_HEADER;

typedef struct http_request_data
{
    HTTP_STATE *state;              /* http 处理的状态 */
    HTTP_REQUEST_HEADER *header;    /* http 报文头 */
    char *body;                     /* http 报文体 */
}HTTP_REQUEST_DATA;

/*--------------------------------------------------*/
/* HTTP 函数
*/
/*--------------------------------------------------*/
int httpServerRun(int port, int pollSize, int pollCoreSize);
int httpServerStartUp(int port, int pollSize, int pollCoreSize, ThreadPool **ppThread_pool, 
                        int *epoll_fd, SERVER_SOCKET *server_socket);
void httpServerRequest(void* arg);

#endif