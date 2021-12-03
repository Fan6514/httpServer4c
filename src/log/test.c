#include "logger.h"

int main()
{
    loggerInit("httpserver");

    LOG_ERROR("log test error");
    LOG_DEBUG("log test debug");
    LOG_INFO("log test info");

    loggerUninit();
    return 0;
}