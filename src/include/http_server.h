#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "util.h"

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_IPADDR_LEN      15
#define MAX_HTTP_HEAD_LEN   1024

/*--------------------------------------------------*/
/* 数据结构 
*/
/*--------------------------------------------------*/
typedef enum { GET = 0, POST, PUT, DELETE, METHOD_NOT_SUPPORT }HTTP_METHOD;
typedef enum { HTTP_10 = 0, HTTP_11, VERSION_NOT_SUPPORT }HTTP_VERSION;
typedef enum { KEEP_ALIVE = 0 }HTTP_CONNECTION;
typedef struct 
{
    char *ip;
    int port;
}HOST_ADDR;

typedef struct http_request_header
{
    HTTP_METHOD method;
    HTTP_VERSION version;
    HOST_ADDR host;
    char *url;
    boolean keep_alive;
}HTTP_REQUEST_HEADER;

typedef struct http_request_data
{
    HTTP_REQUEST_HEADER header;
    /* 后续增加 body 的数据 */
    char *body;
}HTTP_REQUEST_DATA;

/*--------------------------------------------------*/
/* HTTP 函数
*/
/*--------------------------------------------------*/
int httpServerRun(int port, int pollSize, int pollCoreSize);
int httpServerStartUp(int port, int pollSize, int pollCoreSize, ThreadPool **ppThread_pool, 
                        int *epoll_fd, SERVER_SOCKET *server_socket);
void httpServerRequest(void* arg);
int parseHttpData(char *buf, HTTP_REQUEST_DATA *http_data);
int parseHttpRequestHead(char *head_buf, HTTP_REQUEST_DATA *http_data);

#endif