#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "util.h"
#include "socket.h"
#include "thread_pool.h"

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define RETURN_CODE_LEN     4
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

/* http处理状态 */
typedef struct http_state
{
    PARSE_STATE parse_state;        /** @brief 解析报文的状态 */
    RESPONSE_STATE response_state;  /** @brief 报文响应状态 */
}HTTP_STATE;

/* http请求头 */
typedef struct http_request_header
{
    HTTP_METHOD method;             /** @brief 请求方法 */
    HTTP_VERSION version;           /** @brief http 协议版本 */
    char url[MAX_URL_LEN];          /** @brief url */
    char var[MAX_URL_LEN];          /** @brief GET 请求的变量 */
    char host[MAX_VALUE_LEN];       /** @brief 主机地址 */
    BOOLEAN keep_alive;             /** @brief 连接状态 */
    char cookie[MAX_COOKIES_LEN];   /** @brief Cookie */
    int contentLen;                 /** @brief 请求体长度 */
}HTTP_REQUEST_HEADER;

/* http请求报文 */
typedef struct http_request_data
{
    HTTP_STATE state;               /** @brief http 请求处理状态 */
    HTTP_REQUEST_HEADER *header;    /** @brief http 请求报文头 */
    char *body;                     /** @brief http 请求报文体 */
}HTTP_REQUEST_DATA;

/* http响应头 */
typedef struct http_response_header
{
    HTTP_VERSION version;           /** @brief http 协议版本 */
    char rtncode[RETURN_CODE_LEN];  /** @brief http 返回状态码 */
    char reason[MAX_KEY_LEN];       /** @brief http 原因短语 */
    char contentType[MAX_KEY_LEN];  /** @brief http 返回内容类型 */
}HTTP_RESPONSE_HEADER;

/* http响应报文 */
typedef struct http_response_data
{
    HTTP_STATE state;
    HTTP_RESPONSE_HEADER *header;   /** @brief http 响应报文头 */
    char *body;                     /** @brief http 响应报文体 */
}HTTP_RESPONSE_DATA;
/*--------------------------------------------------*/
/* HTTP 函数
*/
/*--------------------------------------------------*/
void httpRequestDataInit(HTTP_REQUEST_DATA **ppHttpRequestData);
void httpRequestDataUninit(HTTP_REQUEST_DATA **ppHttpRequestData);
void httpResponseDataInit(HTTP_RESPONSE_DATA **ppHttpResponseData);
void httpResponseDataUninit(HTTP_RESPONSE_DATA **ppHttpResponseData);

BOOLEAN isUrlHandle(HTTP_REQUEST_HEADER* pReqHead);
int httpServerRequestHandler(HTTP_REQUEST_DATA *pHttpRequestData, HTTP_RESPONSE_DATA *pHttpResponseData);

int constructResponse(HTTP_RESPONSE_DATA *pHttpResponseData, char *buf, int bufLen);
int httpSendResponseMessage(SERVER_SOCKET *server_socket, HTTP_RESPONSE_DATA *pHttpResponseData);

int httpServerRun(int port, int pollSize, int pollCoreSize);
int httpServerStartUp(int port, int pollSize, int pollCoreSize, ThreadPool **ppThread_pool, 
                        int *epoll_fd, SERVER_SOCKET *server_socket);
void httpServerEntry(void* arg);

#endif