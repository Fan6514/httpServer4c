#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdlib.h>
#include "logger.h"
/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_LINE_LEN    1024
#define MAX_USER_NAME_LEN   32
#define MAX_USER_PASSWD_LEN 32
#define MAX_FILE_NAME_LEN   64

/* Define a Boolean data type. */
typedef enum { FALSE, TRUE } __attribute__ ((packed)) BOOLEAN;

#define SUCCESS                 0
#define NOT_FINISH              1
#define SUCCESS_AND_FINISH      2
#define PARA_ERROR              -1
#define RTN_ERROR               -2
#define MEM_ERROR               -3

#define MAX_BUf_LEN         2048

#define REL_MEMORY(ptr)\
do\
{\
    if (NULL != ptr)\
    {\
        free(ptr);\
        ptr = NULL;\
    }\
}while(0)

#define GET_MEMORY(ptr, type, size, pos)\
do\
{\
    ptr = (type*)malloc(size);\
    if (NULL == ptr)\
    {\
        LOG_ERROR("Malloc error.");\
        goto pos;\
    }\
    memset(ptr, 0, size);\
}while(0)

#define GET_MEMORY_RTN(ptr, type, size)\
do\
{\
    ptr = (type*)malloc(size);\
    if (NULL == ptr)\
    {\
        LOG_ERROR("Malloc error.");\
        return MEM_ERROR;\
    }\
    memset(ptr, 0, size);\
}while(0)

#define CHECK_RETURN(ret, sucess, ...)\
do\
{\
    if (sucess != ret)\
    {\
        LOG_ERROR(__VA_ARGS__);\
        return RTN_ERROR;\
    }\
}while(0)

#define CHECK_RETURN_VOID(ret, sucess, ...)\
do\
{\
    if (sucess != ret)\
    {\
        LOG_ERROR(__VA_ARGS__);\
        return;\
    }\
}while(0)

#define CHECK_RETURN_ERR(ret, error, ...)\
do\
{\
    if (error == ret)\
    {\
        LOG_ERROR(__VA_ARGS__);\
        ret = RTN_ERROR;\
    }\
}while(0)

#define CHECK_RETURN_GOTO(ret, sucess, pos, ...)\
do\
{\
    if (sucess != ret)\
    {\
        LOG_ERROR(__VA_ARGS__);\
        goto pos;\
    }\
}while(0)

#define CHECK_POINT(ptr)\
do\
{\
    if (NULL == ptr)\
    {\
        return PARA_ERROR;\
    }\
}while(0)

#define CHECK_POINT_NORTN(ptr)\
do\
{\
    if (NULL == ptr)\
    {\
        return;\
    }\
}while(0)

#define CHECK_EXPRESSION_NORTN(expression, ...)\
do\
{\
    if (expression)\
    {\
        LOG_ERROR(__VA_ARGS__);\
        return;\
    }\
}while(0)

typedef struct config_info
{
    int port;                               /** @brief 端口 */
    int poolSize;                           /** @brief 线程池大小 */
    int poolCoreSize;                       /** @brief 核心线程池大小 */

    int sqlPoolSize;                        /** @brief 数据库连接池数量 */
    char sqlUser[MAX_USER_NAME_LEN];        /** @brief 数据库用户名 */
    char sqlPasswd[MAX_USER_PASSWD_LEN];    /** @brief 数据库用户密码 */
}CONFIG_INFO;

int httpParseReadLine(char *buf, char *pLine, int maxBufSum, int maxLineSum);
void splitStr(char *line, char word[][MAX_LINE_LEN], const char delim, int maxOutNum);
BOOLEAN isSameStr(const char *src, const char *dist);

#endif