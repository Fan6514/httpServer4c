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

/* url处理方法 */
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

/*******************************************************************************
 * @brief       url 初始化函数
 * @return      void
********************************************************************************/
void urlRegInit();

/*******************************************************************************
 * @brief       注册 url
 * @param[in]   urlProc : url处理方法
 * @note        
 * @return      SUCCESS : 注册成功
 *              RTN_ERROR ： 注册失败
********************************************************************************/
int urlRegInsert(URL_PROC_TYPE *urlProc);

/*******************************************************************************
 * @brief       删除 url
 * @param[in]   urlID : url ID
 * @note        
 * @return      SUCCESS : 注册成功
 *              RTN_ERROR ： 注册失败
********************************************************************************/
int urlRegDelete(int urlID);

/*******************************************************************************
 * @brief       查找 url ID
 * @param[in]   url : url 字符串
 * @note        
 * @return      urlID
 *              -1 ： 未找到 urlID
********************************************************************************/
int findUrlId(const char *url);

/*******************************************************************************
 * @brief       返回404，未找到url处理方法
 * @param[in]   arg : 回调入参，HTTP_RESPONSE_DATA类型
 * @note        
 * @return      void
********************************************************************************/
void httpUrlNotFound(void *arg);


#endif