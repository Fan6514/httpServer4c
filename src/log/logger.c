#include <stdio.h>
#include "logger.h"

log4c_category_t* loggerCategory = NULL;

void loggerInit(const char* categoryName)
{
    if(log4c_init() != 0)
    {
        printf("log4c init error.\n");
        return;
    }

    loggerCategory = log4c_category_get(categoryName);
}

void loggerUninit()
{
    if(log4c_fini() != 0)
    {
        printf("log4c uninit error.\n");
    }
}