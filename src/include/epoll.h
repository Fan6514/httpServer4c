#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <sys/epoll.h>

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_EVENT 1024
#define TIME_OUT 1000
#define DEFAULT_EPOLL_EVENTS (EPOLLIN | EPOLLET | EPOLLONESHOT)

/*--------------------------------------------------*/
/* EPOLL 函数
*/
/*--------------------------------------------------*/
int epollInit();
int epollEventAdd(int epoll_fd, int fd);
int epollEventMod(int epoll_fd, int fd, __uint32_t events);
int epollEventDel(int epoll_fd, int fd, __uint32_t events);
int epollWait(int epoll_fd, int *eventSum, struct epoll_event *epoll_events);

#endif