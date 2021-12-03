#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

#include "socket.h"
#include "../include/util.h"

int socketInit(SERVER_SOCKET* http_socket, u_short port)
{
    int on = 1;
    int oldSocketFlag = 0;
    int newSocketFlag = 0;
    int ret = SUCCESS;

    CHECK_POINT(http_socket);

    memset(http_socket, 0, sizeof(SERVER_SOCKET));
    /* 创建监听socket */
    http_socket->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_RETURN_ERR(http_socket->listen_fd, -1, "socket create error.\n");

    /* 设置重用ip地址和端口号 */
    setsockopt(http_socket->listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    setsockopt(http_socket->listen_fd, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on));

    /* 将监听socker设置为非阻塞的 */
    oldSocketFlag = fcntl(http_socket->listen_fd, F_GETFL, 0);
    newSocketFlag = oldSocketFlag | O_NONBLOCK;
    ret = fcntl(http_socket->listen_fd, F_SETFL, newSocketFlag);
    CHECK_RETURN_ERR(ret, -1, "fcntl error.\n");

    /* 初始化服务器地址 */
    http_socket->serverAddr.sin_family = AF_INET;
    http_socket->serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    http_socket->serverAddr.sin_port = htons(port);
    http_socket->clientLen = sizeof(struct sockaddr_in);

    return ret;
}

int socketBind(SERVER_SOCKET* http_socket)
{
    int ret = SUCCESS;

    CHECK_POINT(http_socket);

    ret = bind(http_socket->listen_fd, (struct sockaddr*)&http_socket->serverAddr,
                 sizeof(http_socket->serverAddr));
    CHECK_RETURN_ERR(ret, -1, "bind error.\n");

    return ret;
}

int socketListen(SERVER_SOCKET* http_socket)
{
    int ret = SUCCESS;

    CHECK_POINT(http_socket);

    ret = listen(http_socket->listen_fd, MAX_CONNECTION);
    CHECK_RETURN_ERR(ret, -1, "listen error.\n");

    return ret;
}

int socketAccept(SERVER_SOCKET* http_socket)
{
    CHECK_POINT(http_socket);

    http_socket->conn_fd = accept(http_socket->listen_fd, (struct sockaddr*)&http_socket->clientAddr, 
                                    &http_socket->clientLen);
    CHECK_RETURN_ERR(http_socket->conn_fd, -1, "listen error.\n");
    
    return http_socket->conn_fd == -1 ? -1 : SUCCESS;
}

int socketUninit(SERVER_SOCKET* http_socket)
{
    int ret = SUCCESS;

    CHECK_POINT(http_socket);

    close(http_socket->listen_fd);
    close(http_socket->conn_fd);

    memset(http_socket, 0, sizeof(http_socket));

    return ret;
}

int socketRecv(SERVER_SOCKET* http_socket, char* buf)
{
    int ret = SUCCESS;

    CHECK_POINT(http_socket);

    ret = recv(http_socket->conn_fd, buf, MAX_BUf_LEN, 0);
    printf("%s", buf);

    return ret;
}
