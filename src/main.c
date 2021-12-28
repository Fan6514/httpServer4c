#include <stdio.h>
#include <errno.h>

#include "epoll.h"
#include "socket.h"
#include "thread_pool.h"
#include "http_server.h"
#include "util.h"
#include "url_reg.h"

int main()
{
    loggerInit("httpserver");
    urlRegInit();
    httpServerRun(8000, 100, 50);
    loggerUninit();
    return 0;
}