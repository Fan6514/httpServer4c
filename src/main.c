#include <stdio.h>
#include <errno.h>

#include "epoll.h"
#include "socket.h"
#include "thread_pool.h"
#include "http_server.h"
#include "util.h"
#include "url_reg.h"

void initUrlDispatcher()
{
    int ret = SUCCESS;
    URL_PROC_TYPE urlProcPattern;

    urlRegInit();

    urlProcPattern.urlID = URL_PROC_TEST;
    strncpy(urlProcPattern.url, "/test", MAX_URL_LEN);
    urlProcPattern.urlProcResponse = httpUrlTest;
    ret = urlRegInsert(&urlProcPattern);
}

int main()
{
    loggerInit("httpserver");
    initUrlDispatcher();
    urlRegInit();
    httpServerRun(8000, 100, 50);
    loggerUninit();
    return 0;
}