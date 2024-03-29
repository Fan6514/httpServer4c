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

/* 系统默认的视图处理函数 */
#define URL_PROC_NOT_FOUND          0
#define URL_PROC_NOT_UNIMPLEMENT    1

/* 注册视图函数的 URLID */
#define URL_PROC_BEGIN              10
#define URL_PROC_TEST               URL_PROC_BEGIN

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
    URL_PROC_TYPE views[MAX_URL_PROC_NUM];
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
int urlRegInsert(const char *url, int urlId, (void**)httpViewFunc);

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

/*******************************************************************************
 * @brief       返回501，请求方法不支持
 * @param[in]   arg : 回调入参，HTTP_RESPONSE_DATA类型
 * @note        
 * @return      void
********************************************************************************/
void httpUrlUnimplement(void *arg);

/*******************************************************************************
 * @brief       测试服务器连接
 * @param[in]   arg : 回调入参，HTTP_RESPONSE_DATA类型
 * @note        
 * @return      void
********************************************************************************/
void httpUrlTest(void *arg);

#endif