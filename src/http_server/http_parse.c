/********************************************************************************
 * @file        http_parse.c
 * @author      Fan6514
 * @version     V0.01
 * @date        6-SEP-2021
 * @brief       解析 http 报文内容，包括请求报文的解析和响应报文的封装
 * @htmlonly
 * <span style="font-weight: bold">History</span> 
 * @endhtmlonly 
 * Version|Auther|Date|Describe
 * ------|----|------|-------- 
 * V0.1|Fan|6-SEP-2021|Create File
 * <h2><center>&copy;COPYRIGHT 2021 WELLCASA All Rights Reserved.</center></h2>
********************************************************************************/
#include <stdio.h>
#include <string.h>

#include "http_parse.h"
#include "util.h"

int getMethed(char *method, HTTP_REQUEST_HEADER *pReqHead)
{
    int ret = SUCCESS;

    if (isSameStr(method, "GET"))
    {
        pReqHead->method = GET;
    }
    else if (isSameStr(method, "POST"))
    {
        pReqHead->method = POST;
    }
    else
    {
        pReqHead->method = METHOD_NOT_SUPPORT;
    }

    return ret;
}

int getVersion(char *version, HTTP_REQUEST_HEADER *pReqHead)
{
    int ret = SUCCESS;

    if (isSameStr(version, "HTTP/1.0"))
    {
        pReqHead->version = HTTP_10;
    }
    else if (isSameStr(version, "HTTP/1.1"))
    {
        pReqHead->version = HTTP_11;
    }
    else
    {
        pReqHead->version = VERSION_NOT_SUPPORT;
        ret = PARA_ERROR;
    }

    return ret;
}

/*******************************************************************************
 * @brief       解析获取的 http 请求报文行，包括请求方法、URL、协议版本
 * @param[in]   line : request line
 * @param[out]  http_data : http data struct
 * @note        
 * @return
 *              SUCCESS     解析成功
********************************************************************************/
int parseHttpRequestMsgLine(char *line, HTTP_REQUEST_HEADER *pReqHead)
{
    int readNum = 0;
    int ret = SUCCESS;
    char *urlTemp = NULL;
    char word[3][MAX_LINE_LEN];

    CHECK_POINT(line);
    CHECK_POINT(pReqHead);

    memset(word, 0, sizeof(word));

    /* request method */
    splitStr(line, word, ' ', 3);
    ret = getMethed(word[0], pReqHead);

    /* request url */
    urlTemp = word[1];
    while (*urlTemp != '?' && *urlTemp != '\0')
    {
        urlTemp++;
    }
    if (*urlTemp == '?')
    {
        *urlTemp = '\0';
        urlTemp++;
        strncpy(pReqHead->var, urlTemp, MAX_URL_LEN);
    }
    strncpy(pReqHead->url, word[1], MAX_URL_LEN);

    /* request http version */
    ret = getVersion(word[2], pReqHead);

    return ret;
}

/*******************************************************************************
 * @brief       解析获取的 http 请求报文头，包括host、content-length、cookie等
 * @param[in]   line : buffer
 * @param[out]  http_data : http data struct
 * @note        
 * @return
 *              SUCCESS     解析成功
********************************************************************************/
int parseHttpRequestMsgHead(char *line, HTTP_REQUEST_HEADER *pReqHead)
{
    int ret = SUCCESS;
    char kvBuf[2][MAX_LINE_LEN] = {0};

    CHECK_POINT(line);
    CHECK_POINT(pReqHead);

    splitStr(line, kvBuf, ':', 2);

    if (isSameStr(kvBuf[0], "Host") != 0)
    {
        strncpy(pReqHead->host, kvBuf[1], MAX_VALUE_LEN);
    }
    else if (isSameStr(kvBuf[0], "Connection") != 0)
    {
        if (isSameStr(kvBuf[1], "keep-alive") != 0)
        {
            pReqHead->keep_alive = TRUE;
        }
    }
    else if (isSameStr(kvBuf[0], "Cookies"))
    {
        strncpy(pReqHead->cookie, kvBuf[1], MAX_VALUE_LEN);
    }

    return ret;
}

/*******************************************************************************
 * @brief       解析获取的 http 请求报文的请求正文
 * @param[in]   line : buffer
 * @param[out]  pBody : http data body
 * @note        
 * @return
 *              SUCCESS     解析成功
********************************************************************************/
int parseHttpRequestBody(char *buf, char *pBody)
{
    CHECK_POINT(buf);
    CHECK_POINT(pBody);

    strncat(pBody, buf, MAX_HTTP_BODY_LEN);
    return SUCCESS;
}

/*******************************************************************************
 * @brief       解析获取的 http 报文内容，包括请求报文头和请求体
 * @param[in]   buf : buffer
 * @param[out]  http_data : http data struct
 * @note
 * @return
 *              SUCCESS     解析成功
********************************************************************************/
int parseHttpRequestData(char *buf, HTTP_REQUEST_DATA **ppHttpRequestData)
{
    int readNum = 0;
    int ret = SUCCESS;
    char line[MAX_LINE_LEN];
    char *pBody = NULL;
    char *tempBuf = NULL;
    HTTP_REQUEST_HEADER *pReqHead = NULL;
    PARSE_STATE *state = NULL;

    CHECK_POINT(buf);
    CHECK_POINT(ppHttpRequestData);
    CHECK_POINT(*ppHttpRequestData);
    CHECK_POINT((*ppHttpRequestData)->header);

    memset(line, 0, MAX_LINE_LEN);
    pReqHead = (*ppHttpRequestData)->header;
    state = &(*ppHttpRequestData)->state.parse_state;
    pBody = (*ppHttpRequestData)->body;
    tempBuf = buf;

    /* 解析报文内容 */
    while (*tempBuf != '\0')
    {
        readNum = httpParseReadLine(tempBuf, line, MAX_BUf_LEN, MAX_LINE_LEN);
        LOG_INFO("Read line: %s, Line size: %d", line, readNum);
        tempBuf += readNum;

        if (*line == '\n' || *line == '\r')
        {
            *state = PARSE_REQUEST_BODY;
            continue;
        }

        switch (*state)
        {
            case PARSE_REQUEST_LINE:
                ret = parseHttpRequestMsgLine(line, pReqHead);
                if (SUCCESS == ret) { *state = PARSE_REQUEST_HEAD; }
                break;
            case PARSE_REQUEST_HEAD:
                ret = parseHttpRequestMsgHead(line, pReqHead);
                break;
            case PARSE_REQUEST_BODY:
                ret = parseHttpRequestBody(tempBuf, pBody);
                *state = PARSE_COMPLATE;
                LOG_INFO("[httpServer] body:%s\n", pBody);
                break;
            default:
                break;
        }
        memset(line, 0, MAX_LINE_LEN);
    }

    if (GET == pReqHead->method)
    {
        *state = PARSE_COMPLATE;
    }
    LOG_INFO("[httpServer] Request data parse finish, state:%d.", *state);

finish:
    return ret;
}