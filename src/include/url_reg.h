#ifndef _URL_REG_H_
#define _URL_REG_H_

#include "util.h"
#include "http_server.h"

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/

/* / */
#define ROOT_URL        0

/* url string */
#define ROOT_URL_STR    "/"
/*--------------------------------------------------*/
/* 数据结构 
*/
/*--------------------------------------------------*/
typedef struct url_proc_type
{
    int urlID;                  /* url 注册id */
    char url[MAX_URL_LEN];      /* url */
    void *urlProcResponse;      /* url 处理函数 */
}URL_PROC_TYPE;

/*--------------------------------------------------*/
/* url 函数
*/
/*--------------------------------------------------*/

#endif