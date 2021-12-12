#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#include "epoll.h"
#include "http_server.h"
#include "http_parse.h"
#include "util.h"

extern struct epoll_event *events;

void httpServerDataInit(HTTP_REQUEST_DATA **ppHttp_data)
{
    char *body = NULL;
    HTTP_REQUEST_HEADER *pHeader = NULL;

    CHECK_POINT_NORTN(ppHttp_data);
    CHECK_POINT_NORTN(*ppHttp_data);

    GET_MEMORY(body, char, MAX_HTTP_BODY_LEN, error);
    GET_MEMORY(pHeader, HTTP_REQUEST_HEADER, sizeof(HTTP_REQUEST_HEADER), error);

    (*ppHttp_data)->state = pState;
    (*ppHttp_data)->header = pHeader;
    (*ppHttp_data)->body = body;

error:
    REL_MEMORY(body);
    REL_MEMORY(pHeader);
    REL_MEMORY(pState);
}

void httpServerDataUninit(HTTP_REQUEST_DATA **ppHttp_data)
{
    REL_MEMORY((*ppHttp_data)->state);
    REL_MEMORY((*ppHttp_data)->header);
    REL_MEMORY((*ppHttp_data)->body);
}

/*******************************************************************************
 * @brief       http 请求处理函数
 * @param[in]   arg : arg
 * @note        
 * @return
********************************************************************************/
void httpServerRequest(void *arg)
{
    int ret = 0;
    char *buf = NULL;
    SERVER_SOCKET *server_socket = NULL;
    HTTP_REQUEST_DATA *pHttp_data = NULL;
    HTTP_REQUEST_HEADER *pHeader = NULL;

    CHECK_POINT_NORTN(arg);
    
    server_socket = (SERVER_SOCKET *)arg;
    GET_MEMORY(buf, char, MAX_BUf_LEN, finish);
    GET_MEMORY(pHttp_data, HTTP_REQUEST_DATA, sizeof(HTTP_REQUEST_DATA), finish);
    httpServerDataInit(&pHttp_data);
    LOG_DEBUG("%d\n", pHttp_data->state->parse_state);

    pHeader = pHttp_data->header;

    while(true)
    {
        ret = socketRecv(server_socket, buf);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] socket recv msg error.");
        ret = parseHttpData(buf, pHttp_data);
        CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] socket parse http data error.");

        switch(pHeader->method)
        {
            case GET:
                //do_requestForGet();
            case POST:
                //do_requestForPost();
            default:
                break;
        }
    }

finish:
    REL_MEMORY(buf);
    httpServerDataUninit(&pHttp_data);
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

int httpServerRun(int port, int pollSize, int pollCoreSize)
{
    int ret = SUCCESS;
    int epoll_fd = 0;
    int eventSum = 0;
    SERVER_SOCKET server_socket;
    ThreadPool *thread_pool = NULL;

    /* 服务器初始化 */
    ret = httpServerStartUp(port, pollSize, pollCoreSize, &thread_pool, &epoll_fd, &server_socket);
    CHECK_RETURN_GOTO(ret, SUCCESS, error, "httpServerStartUp error.");

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

            ret = threadPoolAddTask(thread_pool, httpServerRequest, (void*)&server_socket);
            CHECK_RETURN(ret, SUCCESS, "threadPoolAddTask error.");
        }
    }

error:
    socketUninit(&server_socket);
    threadPoolDestroy(thread_pool);
    return ret;
}