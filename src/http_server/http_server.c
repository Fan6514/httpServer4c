#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#include "epoll.h"
#include "http_server.h"
#include "http_parse.h"
#include "util.h"

extern struct epoll_event *events;

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
    pReqHeader->keep_alive = false;
    pReqHeader->method = METHOD_NOT_SUPPORT;
    pReqHeader->version = VERSION_NOT_SUPPORT;

    (*ppHttpRequestData)->state.parse_state = PARSE_REQUEST_LINE;
    (*ppHttpRequestData)->header = pReqHeader;
    (*ppHttpRequestData)->body = requestBody;

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
    pRspHeader->rtncode = RETURN_STATE_NOT_FUND;
    pRspHeader->version = VERSION_NOT_SUPPORT;

    (*ppHttpResponseData)->state.response_state = UNCOMPLATE;
    (*ppHttpResponseData)->header = pRspHeader;
    (*ppHttpResponseData)->body = responseBody;

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

    /* 初始化请求报文 */
    httpRequestDataInit(&pHttpRequestData);
    /* 初始化响应报文 */
    httpResponseDataInit(&pHttpResponseData);    

    while(true)
    {
        /* 接收数据 */
        ret = socketRecv(server_socket, buf);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] socket recv msg error.");
        /* 解析请求报文 */
        ret = parseHttpRequestData(buf, &pHttpRequestData);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] parse http request data error.");
        /* 处理请求报文 */
        ret = httpServerHandler(pHttpRequestData, pHttpResponseData);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] handle http request data error.");
        /* 发送响应 */
        ret = httpSendResponse(server_socket, pHttpResponseData);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] socket parse http data error.");
    }

finish:
    REL_MEMORY(buf);
    httpRequestDataUninit(&pHttpRequestData);
    httpResponseDataUninit(&pHttpResponseData);
}

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

    while(true)
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