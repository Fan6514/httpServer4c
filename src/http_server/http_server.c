#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#include "epoll.h"
#include "socket.h"
#include "thread_pool.h"
#include "http_server.h"
#include "util.h"

extern struct epoll_event *events;

int parseHttpRequestHead(char *head_buf, HTTP_REQUEST_DATA *http_data)
{
    int ret = SUCCESS;

    CHECK_POINT(head_buf);
    CHECK_POINT(http_data);

    
}

int parseHttpData(char *buf, HTTP_REQUEST_DATA *http_data)
{
    int nPos = 0;
    int ret = SUCCESS;
    char *head_buf = NULL;
    char *temp_buf = NULL;

    CHECK_POINT(buf);
    CHECK_POINT(http_data);

    memset(http_data, 0, sizeof(HTTP_REQUEST_DATA));
    GET_MEMORY(head_buf, char, MAX_HTTP_HEAD_LEN, finish);

    /* 获取报文头 */
    temp_buf = buf;
    while (*temp_buf != '\0')
    {
        if (temp_buf == strstr(temp_buf, "\r\n\r\n"))
        {
            break;
        }
        nPos++;
        temp_buf++;
    }
    strncpy(head_buf, buf, nPos);
    LOG_DEBUG("[httpServer] head_buf:%s", head_buf);

    ret = parseHttpRequestHead(head_buf, http_data);
    CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] parseHttpRequestHead error.");
    LOG_INFO("[httpServer] method:%d, version:%d, url:%s", http_data->header.method, http_data->header.version, 
                                                            http_data->header.url);

finish:
    REL_MEMORY(head_buf);
    return ret;
}

void httpServerRequest(void *arg)
{
    int ret = 0;
    char *buf = NULL;
    SERVER_SOCKET *server_socket = NULL;
    HTTP_REQUEST_DATA http_data;

    CHECK_POINT_NORTN(arg);
    
    server_socket = (SERVER_SOCKET *)arg;
    GET_MEMORY(buf, char, MAX_BUf_LEN, finish);

    ret = socketRecv(server_socket, buf);
    CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] socket recv msg error.");
    ret = parseHttpData(buf, &http_data);
    CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] socket parse http data error.");

    switch(http_data.header.method)
    {
        case GET:
            //do_requestForGet();
        case POST:
            //do_requestForPost();
    }

finish:
    REL_MEMORY(buf);
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