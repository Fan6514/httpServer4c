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
    gRegUrls.regNum = 0;
    for (int i = 0; i < MAX_URL_PROC_NUM; ++i)
    {
        gRegUrls.urls[i].urlID = -1;
        gRegUrls.urls[i].urlProcResponse = NULL;
        memset(gRegUrls.urls[i].url, 0, MAX_URL_LEN);
    }

    return;
}

int urlRegInsert(URL_PROC_TYPE *urlProc)
{
    int urlId = 0;
    int ret = SUCCESS;

    CHECK_POINT(urlProc);

    urlId = urlProc->urlID;
    if (urlId < URL_PROC_BEGIN || urlId >= MAX_URL_PROC_NUM)
    {
        LOG_ERROR("[url_reg] insert url proccess irlID:%d error", urlId);
        return RTN_ERROR;
    }

    gRegUrls.urls[gRegUrls.regNum].urlID = urlProc->urlID;
    gRegUrls.urls[gRegUrls.regNum].urlProcResponse = urlProc->urlProcResponse;
    strncpy(gRegUrls.urls[gRegUrls.regNum].url, urlProc->url, MAX_URL_LEN);

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
        if (gRegUrls.urls[i].urlID == urlID)
        {
            gRegUrls.urls[i].urlID = -1;
            gRegUrls.urls[i].urlProcResponse = NULL;
            memset(gRegUrls.urls[i].url, 0, MAX_URL_LEN);
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
        if (isSameStr(gRegUrls.urls[i].url, url))
        {
            urlId = gRegUrls.urls[i].urlID;
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
