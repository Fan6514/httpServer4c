#include <stdio.h>
#include <errno.h>

#include "epoll.h"
#include "socket.h"
#include "thread_pool.h"
#include "util.h"

int main()
{
    loggerInit("httpserver");
    httpServerRun(8000, 100, 50);
    loggerUninit();
    return 0;
}