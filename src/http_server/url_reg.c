/********************************************************************************
 * @file        url_reg.c
 * @author      Fan6514
 * @version     V0.01
 * @date        20-DEC-2021
 * @brief       url 注册文件
 * @htmlonly
 * <span style="font-weight: bold">History</span> 
 * @endhtmlonly 
 * Version|Auther|Date|Describe
 * ------|----|------|-------- 
 * V0.1|Fan|20-DEC-2021|Create File
 * <h2><center>&copy;COPYRIGHT 2021 WELLCASA All Rights Reserved.</center></h2>
********************************************************************************/
#include "url_reg.h"
#include "http_server.h"
#include <string.h>

URL_REG_TYPE gRegUrls;

void urlRegInit()
{
    URL_PROC_TYPE urlProcPattern;

    gRegUrls.regNum = URL_PROC_BEGIN;
    for (int i = 0; i < MAX_URL_PROC_NUM; ++i)
    {
        gRegUrls.views[i].urlID = -1;
        gRegUrls.views[i].urlProcResponse = NULL;
        memset(gRegUrls.views[i].url, 0, MAX_URL_LEN);
    }

    gRegUrls.views[URL_PROC_NOT_FOUND].urlID = URL_PROC_NOT_FOUND;
    gRegUrls.views[URL_PROC_NOT_FOUND].urlProcResponse = (void*)httpUrlNotFound;

    gRegUrls.views[URL_PROC_NOT_UNIMPLEMENT].urlID = URL_PROC_NOT_UNIMPLEMENT;
    gRegUrls.views[URL_PROC_NOT_UNIMPLEMENT].urlProcResponse = (void*)httpUrlUnimplement;

    gRegUrls.views[URL_PROC_TEST].urlID = URL_PROC_TEST;
    gRegUrls.views[URL_PROC_TEST].urlProcResponse = (void*)httpUrlTest;
    strncpy(gRegUrls.views[URL_PROC_TEST].url, "/test", MAX_URL_LEN);

    urlRegInsert("/test", URL_PROC_TEST, &httpUrlTest);

    return;
}

int urlRegInsert(const char *url, int urlId, (void**)httpViewFunc)
{
    int ret = SUCCESS;
    URL_PROC_TYPE *view;

    CHECK_POINT(httpViewFunc);
    CHECK_POINT(*httpViewFunc);
    if (urlId < URL_PROC_BEGIN || urlId >= MAX_URL_PROC_NUM)
    {
        LOG_ERROR("[url_reg] insert url proccess irlID:%d error", urlId);
        return RTN_ERROR;
    }

    view = &gRegUrls.views[gRegUrls.regNum];

    view->urlID = urlId;
    view->urlProcResponse = *httpViewFunc;
    strncpy(view->url, url, MAX_URL_LEN);

    gRegUrls.regNum++;

    return ret;
}

int urlRegDelete(int urlID)
{
    int ret = SUCCESS;

    if (urlID < URL_PROC_BEGIN || urlID >= MAX_URL_PROC_NUM)
    {
        return RTN_ERROR;
    }

    for (int i = URL_PROC_BEGIN; i < MAX_URL_PROC_NUM; ++i)
    {
        if (gRegUrls.views[i].urlID == urlID)
        {
            gRegUrls.views[i].urlID = -1;
            gRegUrls.views[i].urlProcResponse = NULL;
            memset(gRegUrls.views[i].url, 0, MAX_URL_LEN);
            gRegUrls.regNum--;
            break;
        }
    }
}

int findUrlId(const char *url)
{
    int urlId = 0;

    CHECK_POINT(url);

    for (int i = URL_PROC_BEGIN; i < MAX_URL_PROC_NUM; ++i)
    {
        if (isSameStr(gRegUrls.views[i].url, url))
        {
            urlId = gRegUrls.views[i].urlID;
            return urlId;
        }
    }

    return -1;
}

void httpUrlNotFound(void *arg)
{
    HTTP_RESPONSE_HEADER *pRspHead = NULL;
    HTTP_RESPONSE_DATA *pHttpResponseData = NULL;
    char resBuf[RESPONSE_BUFFER_SUM];

    CHECK_POINT_NORTN(arg);

    pHttpResponseData = (HTTP_RESPONSE_DATA*)arg;
    pRspHead = pHttpResponseData->header;
    memset(resBuf, 0, RESPONSE_BUFFER_SUM);

    /* 处理报文头 */
    strncpy(pRspHead->rtncode, "404", RETURN_CODE_LEN);
    strncpy(pRspHead->reason, "NOT FOUND", sizeof(pRspHead->reason));
    strncpy(pRspHead->contentType, "text/html", sizeof(pRspHead->contentType));

    /* 处理报文体 */
    strncpy(resBuf, "<HTML><TITLE>Not Found</TITLE>\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "<BODY><P>The server could not fulfill\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "your request because the resource specified\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "is unavailable or nonexistent.\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "</BODY></HTML>\r\n", RESPONSE_BUFFER_SUM);
    strncpy(pHttpResponseData->body, resBuf, MAX_HTTP_BODY_LEN);
}

void httpUrlUnimplement(void *arg)
{
    HTTP_RESPONSE_HEADER *pRspHead = NULL;
    HTTP_RESPONSE_DATA *pHttpResponseData = NULL;
    char resBuf[RESPONSE_BUFFER_SUM];

    CHECK_POINT_NORTN(arg);

    pHttpResponseData = (HTTP_RESPONSE_DATA*)arg;
    pRspHead = pHttpResponseData->header;
    memset(resBuf, 0, RESPONSE_BUFFER_SUM);

    /* 处理报文头 */
    strncpy(pRspHead->rtncode, "501", RETURN_CODE_LEN);
    strncpy(pRspHead->reason, "Method Not Implemented", sizeof(pRspHead->reason));
    strncpy(pRspHead->contentType, "text/html", sizeof(pRspHead->contentType));

    /* 处理报文体 */
    strncpy(resBuf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "</TITLE></HEAD>\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "<BODY><P>HTTP request method not supported.\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "</BODY></HTML>\r\n", RESPONSE_BUFFER_SUM);
    strncpy(pHttpResponseData->body, resBuf, MAX_HTTP_BODY_LEN); 
}

void httpUrlTest(void *arg)
{
    HTTP_RESPONSE_HEADER *pRspHead = NULL;
    HTTP_RESPONSE_DATA *pHttpResponseData = NULL;
    char resBuf[RESPONSE_BUFFER_SUM];

    CHECK_POINT_NORTN(arg);

    pHttpResponseData = (HTTP_RESPONSE_DATA*)arg;
    pRspHead = pHttpResponseData->header;
    memset(resBuf, 0, RESPONSE_BUFFER_SUM);

    /* 处理报文头 */
    strncpy(pRspHead->rtncode, "200", RETURN_CODE_LEN);
    strncpy(pRspHead->reason, "OK", sizeof(pRspHead->reason));
    strncpy(pRspHead->contentType, "text/html", sizeof(pRspHead->contentType));

    /* 处理报文体 */
    strncpy(resBuf, "<HTML><HEAD><TITLE>Test page\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "</TITLE></HEAD>\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "<BODY><P>HTTP request test page.\r\n", RESPONSE_BUFFER_SUM);
    strncat(resBuf, "</BODY></HTML>\r\n", RESPONSE_BUFFER_SUM);
    strncpy(pHttpResponseData->body, resBuf, MAX_HTTP_BODY_LEN); 
}
