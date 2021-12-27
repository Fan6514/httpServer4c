#ifndef _URL_REG_H_
#define _URL_REG_H_

#include "util.h"
#include "http_server.h"

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_URL_PROC_NUM            1024
#define RESPONSE_BUFFER_SUM         1024

/* 系统默认的处理函数 */
#define URL_PROC_NOT_FOUND          0
#define URL_PROC_NOT_UNIMPLEMENT    1

/* web处理url函数 */
#define URL_PROC_BEGIN              10
/*--------------------------------------------------*/
/* 数据结构 
*/
/*--------------------------------------------------*/
typedef struct url_proc_type
{
    int urlID;                              /** @brief url 注册id */
    char url[MAX_URL_LEN];                  /** @brief url */
    void (*urlProcResponse)(void *arg);     /** @brief url 处理函数 */
}URL_PROC_TYPE;

typedef struct url_reg_type
{
    int regNum;                             /** @brief 注册个数 */
    URL_PROC_TYPE urls[MAX_URL_PROC_NUM];
}URL_REG_TYPE;


/*--------------------------------------------------*/
/* url 函数
*/
/*--------------------------------------------------*/

#endif