#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_CONNECTION 1024
#define MAX_BUf_LEN 2048
/*--------------------------------------------------*/
/* 数据结构 
*/
/*--------------------------------------------------*/
typedef struct serverSocket
{
    int listen_fd;
    int conn_fd;
    socklen_t clientLen;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    u_short port;
}SERVER_SOCKET;

/*--------------------------------------------------*/
/* SOCKET 函数
*/
/*--------------------------------------------------*/
int socketInit(SERVER_SOCKET* http_socket, u_short port);
int socketBind(SERVER_SOCKET* http_socket);
int socketListen(SERVER_SOCKET* http_socket);
int socketAccept(SERVER_SOCKET* http_socket);
int socketUninit(SERVER_SOCKET* http_socket);
int socketRecv(SERVER_SOCKET* http_socket);

#endif