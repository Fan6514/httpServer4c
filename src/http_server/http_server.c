#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#include "epoll.h"
#include "http_server.h"
#include "http_parse.h"
#include "util.h"
#include "url_reg.h"

extern struct epoll_event *events;
extern URL_REG_TYPE gRegUrls;

/*******************************************************************************
 * @brief : 初始化请求报文
 * @param [out] ppHttpRequestData
********************************************************************************/
void httpRequestDataInit(HTTP_REQUEST_DATA **ppHttpRequestData)
{
    char *requestBody = NULL;
    HTTP_REQUEST_HEADER *pReqHeader = NULL;

    CHECK_POINT_NORTN(ppHttpRequestData);
    CHECK_POINT_NORTN(*ppHttpRequestData);

    GET_MEMORY(requestBody, char, MAX_HTTP_BODY_LEN, error);
    GET_MEMORY(pReqHeader, HTTP_REQUEST_HEADER, sizeof(HTTP_REQUEST_HEADER), error);

    /* 初始化请求报文头 */
    pReqHeader->keep_alive = FALSE;
    pReqHeader->method = METHOD_NOT_SUPPORT;
    pReqHeader->version = VERSION_NOT_SUPPORT;

    (*ppHttpRequestData)->state.parse_state = PARSE_REQUEST_LINE;
    (*ppHttpRequestData)->header = pReqHeader;
    (*ppHttpRequestData)->body = requestBody;

    return;

error:
    REL_MEMORY(requestBody);
    REL_MEMORY(pReqHeader);
}

void httpRequestDataUninit(HTTP_REQUEST_DATA **ppHttpRequestData)
{
    REL_MEMORY((*ppHttpRequestData)->header);
    REL_MEMORY((*ppHttpRequestData)->body);
    REL_MEMORY(*ppHttpRequestData);
}

/*******************************************************************************
 * @brief : 初始化响应报文
 * @param [out] ppHttpResponseData
********************************************************************************/
void httpResponseDataInit(HTTP_RESPONSE_DATA **ppHttpResponseData)
{
    char *responseBody = NULL;
    HTTP_RESPONSE_HEADER *pRspHeader = NULL;

    CHECK_POINT_NORTN(ppHttpResponseData);
    CHECK_POINT_NORTN(*ppHttpResponseData);

    GET_MEMORY(responseBody, char, MAX_HTTP_BODY_LEN, error);
    GET_MEMORY(pRspHeader, HTTP_RESPONSE_HEADER, sizeof(HTTP_RESPONSE_HEADER), error);

    /* 初始化请求报文头 */
    pRspHeader->version = VERSION_NOT_SUPPORT;

    (*ppHttpResponseData)->state.response_state = UNCOMPLATE;
    (*ppHttpResponseData)->header = pRspHeader;
    (*ppHttpResponseData)->body = responseBody;

    return;

error:
    REL_MEMORY(responseBody);
    REL_MEMORY(pRspHeader);
}

void httpResponseDataUninit(HTTP_RESPONSE_DATA **ppHttpResponseData)
{
    REL_MEMORY((*ppHttpResponseData)->header);
    REL_MEMORY((*ppHttpResponseData)->body);
    REL_MEMORY(*ppHttpResponseData);
}

/*******************************************************************************
 * @brief       是否需要由url处理
 * @param[in]   pReqHead : 请求报文头
 * @note        当GET请求有变量，或POST请求，则由cgi处理
 * @return
 *              TRUE / FALSE
********************************************************************************/
BOOLEAN isUrlHandle(HTTP_REQUEST_HEADER* pReqHead)
{
    if (pReqHead == NULL) { return FALSE; }
    return ((POST == pReqHead->method) || (GET == pReqHead->method && pReqHead->var[0] != '\0'));
}

/*******************************************************************************
 * @brief       处理请求的url
 * @param[in]   pHttpRequestData : 请求报文
 * @param[out]  pHttpResponseData : 响应报文
 * @note        
 * @return
 *              SUCCESS     处理成功
********************************************************************************/
int httpServerRequestHandler(HTTP_REQUEST_DATA *pHttpRequestData, HTTP_RESPONSE_DATA *pHttpResponseData)
{
    int ret = 0;
    int urlId = 0;
    char url[MAX_URL_LEN] = {0};
    char arg[MAX_URL_LEN] = {0};
    HTTP_REQUEST_HEADER *pRequestHead = NULL;
    HTTP_RESPONSE_HEADER *pResponseHead = NULL;

    CHECK_POINT(pHttpRequestData);
    CHECK_POINT(pHttpResponseData);

    pResponseHead = pHttpResponseData->header;
    pRequestHead = pHttpRequestData->header;
    pResponseHead->version = pRequestHead->version;

    LOG_DEBUG("httpServerRequestHandler start.");

    if (METHOD_NOT_SUPPORT == pRequestHead->method)
    {/* 501请求不存在 */
        gRegUrls.urls[URL_PROC_NOT_UNIMPLEMENT].urlProcResponse((void *)pHttpResponseData);
        return ret;
    }

    if(isUrlHandle(pRequestHead))
    {
        urlId = findUrlId(url);
        if (urlId < 0)
        {/* not found */
            LOG_DEBUG("Request url is not found! urlID=%d", urlId);
            gRegUrls.urls[URL_PROC_NOT_FOUND].urlProcResponse((void *)pHttpResponseData);
            return ret;
        }
        /* 处理对应 url */
        gRegUrls.urls[urlId].urlProcResponse((void *)arg);
    }
    else
    {
        gRegUrls.urls[URL_PROC_NOT_FOUND].urlProcResponse((void *)pHttpResponseData);
    }

    LOG_DEBUG("httpServerRequestHandler finish.");
    return ret;
}

/*******************************************************************************
 * @brief       构造http响应报文字符串
 * @param[in]   pHttpResponseData : 响应报文
 * @param[in]   buf : 缓冲区
 * @note        
 * @return
 *              SUCCESS     处理成功
********************************************************************************/
int constructResponse(HTTP_RESPONSE_DATA *pHttpResponseData, char *buf, int bufLen)
{
    int ret = 0;
    char *tempBuf = NULL;
    HTTP_RESPONSE_HEADER *pResHead = NULL;

    pResHead = pHttpResponseData->header;
    tempBuf = buf;

    if (HTTP_10 == pResHead->version)
    {
        strncpy(tempBuf, "HTTP/1.0 ", bufLen);
    }
    else
    {
        strncpy(tempBuf, "HTTP/1.1 ", bufLen);
    }

    strncat(tempBuf, pResHead->rtncode, bufLen);
    strncat(tempBuf, " ", bufLen);
    strncat(tempBuf, pResHead->reason, bufLen);
    strncat(tempBuf, "\r\n", bufLen);
    strncat(tempBuf, "Content-Type: ", bufLen);
    strncat(tempBuf, pResHead->contentType, bufLen);
    strncat(tempBuf, "\r\n", bufLen);
    strncat(tempBuf, "\r\n", bufLen);
    strncat(tempBuf, pHttpResponseData->body, bufLen);
    
    return ret;
}

/*******************************************************************************
 * @brief       发送http响应报文
 * @param[in]   server_socket : 服务器socket
 * @param[in]   pHttpResponseData : 响应报文
 * @note        
 * @return
 *              SUCCESS     处理成功
********************************************************************************/
int httpSendResponseMessage(SERVER_SOCKET *server_socket, HTTP_RESPONSE_DATA *pHttpResponseData)
{
    int ret = 0;
    char *buf = NULL;

    CHECK_POINT(server_socket);
    CHECK_POINT(pHttpResponseData);

    GET_MEMORY(buf, char, MAX_BUf_LEN, finish);

    ret = constructResponse(pHttpResponseData, buf, MAX_BUf_LEN);
    CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] constructResponse error.");
    LOG_DEBUG("Construct response message:\n%s\n", buf);

    ret = socketSend(server_socket, buf);

finish:
    REL_MEMORY(buf);
    return ret;
}

/*******************************************************************************
 * @brief       http 入口函数
 * @param[in]   arg : SERVER_SOCKET 类型，包括服务器和客户端套接字
 * @note        
 * @return
********************************************************************************/
void httpServerEntry(void *arg)
{
    int ret = 0;
    char *buf = NULL;
    SERVER_SOCKET *server_socket = NULL;
    HTTP_REQUEST_DATA *pHttpRequestData = NULL;
    HTTP_RESPONSE_DATA *pHttpResponseData = NULL;

    CHECK_POINT_NORTN(arg);
    
    server_socket = (SERVER_SOCKET *)arg;
    GET_MEMORY(buf, char, MAX_BUf_LEN, finish);
    GET_MEMORY(pHttpRequestData, HTTP_REQUEST_DATA, sizeof(HTTP_REQUEST_DATA), finish);
    GET_MEMORY(pHttpResponseData, HTTP_RESPONSE_DATA, sizeof(HTTP_RESPONSE_DATA), finish);

    /* 初始化请求报文 */
    httpRequestDataInit(&pHttpRequestData);
    /* 初始化响应报文 */
    httpResponseDataInit(&pHttpResponseData);

    while(TRUE)
    {
        /* 接收数据 */
        ret = socketRecv(server_socket, buf);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] socket recv msg error.");
        /* 解析请求报文 */
        ret = parseHttpRequestData(buf, &pHttpRequestData);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] parse http request data error.");
        if (PARSE_COMPLATE != pHttpRequestData->state.parse_state)
        {
            continue;
        }

        /* 处理请求报文 */
        ret = httpServerRequestHandler(pHttpRequestData, pHttpResponseData);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] handle http request data error, retCode=%d.", ret);
        /* 发送响应 */
        ret = httpSendResponseMessage(server_socket, pHttpResponseData);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] socket parse http data error, retCode=%d.", ret);
        
        if (SUCCESS == ret)
        {
            pHttpRequestData->state.parse_state = PARSE_REQUEST_LINE;
            break;
        }
    }

finish:
    REL_MEMORY(buf);
    httpRequestDataUninit(&pHttpRequestData);
    httpResponseDataUninit(&pHttpResponseData);
}

/*******************************************************************************
 * @brief       服务器上电初始化
 * @param[in]   port : 启动端口
 * @param[in]   pollSize : 线程池大小
 * @param[in]   pollCoreSize : 核心线程池大小
 * @param[out]   ppThread_pool : 线程池
 * @param[out]   epoll_fd : epoll句柄
 * @param[out]   server_socket : 服务器socket
 * @note        
 * @return
 *              SUCCESS     上电成功
********************************************************************************/
int httpServerStartUp(int port, int pollSize, int pollCoreSize, ThreadPool **ppThread_pool, 
                        int *epoll_fd, SERVER_SOCKET *server_socket)
{
    int ret = SUCCESS;

    CHECK_POINT(ppThread_pool);
    CHECK_POINT(epoll_fd);
    CHECK_POINT(server_socket);

    /* 初始化线程池 */
    *ppThread_pool = threadPoolCreate(MAX_CONNECTION, pollSize, pollCoreSize);
    CHECK_POINT(*ppThread_pool);
    LOG_INFO("[threadPool create] poolSize: %d, poolCoreSize:%d", pollSize, pollCoreSize);

    /* 初始化 socket */
    ret = socketInit(server_socket, port);
    CHECK_RETURN_ERR(ret, -1, "socketInit error.");
    /* bind */
    ret = socketBind(server_socket);
    CHECK_RETURN_ERR(ret, -1, "socketBind error.");
    /* listen */
    ret = socketListen(server_socket);
    CHECK_RETURN_ERR(ret, -1, "socketListen error.");

    /* 初始化 epoll */
    *epoll_fd = epollInit();
    CHECK_RETURN_ERR(*epoll_fd, -1, "epollInit error.");
    ret = epollEventAdd(*epoll_fd, server_socket->listen_fd);
    CHECK_RETURN_ERR(ret, -1, "epollEventAdd error.");

    return ret;
}

/*******************************************************************************
 * @brief       服务器运行函数
 * @param[in]   port : 启动端口
 * @param[in]   pollSize : 线程池大小
 * @param[in]   pollCoreSize : 核心线程池大小
 * @note        
 * @return
 *              SUCCESS     下电成功
********************************************************************************/
int httpServerRun(int port, int pollSize, int pollCoreSize)
{
    int ret = SUCCESS;
    int epoll_fd = 0;
    int eventSum = 0;
    SERVER_SOCKET server_socket;
    ThreadPool *thread_pool = NULL;

    /* 服务器初始化 */
    ret = httpServerStartUp(port, pollSize, pollCoreSize, &thread_pool, &epoll_fd, &server_socket);
    CHECK_RETURN_GOTO(ret, SUCCESS, finish, "httpServerStartUp error.");

    while(TRUE)
    {
        ret = epollWait(epoll_fd, &eventSum, events);
        if (SUCCESS != ret)
        {
            /* 信号中断或超时 */
            if (errno == EINTR || eventSum == 0) { continue; }
            else { break; }
        }
        
        for (int i = 0; i < eventSum; ++i)
        {
            ret = socketAccept(&server_socket);
            CHECK_RETURN(ret, SUCCESS, "socketAccept error.");
            LOG_INFO("[socket]connect:%d port:%u client addr:%s", server_socket.conn_fd, server_socket.port,
                                                    inet_ntoa(server_socket.clientAddr.sin_addr));

            ret = threadPoolAddTask(thread_pool, httpServerEntry, (void*)&server_socket);
            CHECK_RETURN(ret, SUCCESS, "threadPoolAddTask error.");
        }
    }

finish:
    socketUninit(&server_socket);
    threadPoolDestroy(thread_pool);
    return ret;
}