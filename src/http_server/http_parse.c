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

/*******************************************************************************
 * @brief       读取缓冲区内的一行内容
 * @param[in]   buf : buffer
 * @param[out]  pLine : the first line in buf
 * @param[in]   maxBufSum : the max buf size
 * @param[in]   maxLineSum : the max line size
 * @note        输出参数 pLine 以 '\0' 结尾，包含 '\n'.
 * @return
 *              PARA_ERROR  入参错误
 *              lineIndex   读取行的字符数
********************************************************************************/
int httpParseReadLine(char *buf, char *pLine, int maxBufSum, int maxLineSum)
{
    int lineIndex = 0;

    CHECK_POINT(buf);
    CHECK_POINT(pLine);
    if (maxBufSum <= 0 || maxLineSum <= 0)
    {
        return PARA_ERROR;
    }

    /* 跳过字符串开头的空行和'\n' */
    while(*buf == ' ' || *buf == '\n')
    {
        buf++;
    }

    while (*buf != '\0' && *buf != '\n' && lineIndex < maxLineSum -1)
    {
        pLine[lineIndex] = *buf;
        lineIndex++;
        buf++;
    }
    pLine[lineIndex] = '\0';
    lineIndex++;
    
    return lineIndex;
}

int splitStr(char *line, char (*word)[MAX_LINE_LEN], const char *delim)
{
    int wordIndex = 0;
    char *subStr = NULL;
    char *lineTemp = NULL;

    CHECK_POINT(line);
    CHECK_POINT(word);

    while ((subStr = strtok_r(line, delim, &lineTemp)) != NULL)
    {
        strncpy(word[wordIndex], subStr, MAX_LINE_LEN);
        wordIndex++;
    }

    return wordIndex;
}

/*******************************************************************************
 * @brief       解析获取的 http 请求报文头，包括host、content-length、cookie等
 * @param[in]   line : buffer
 * @param[out]  http_data : http data struct
 * @note        
 * @return
 *              SUCCESS     解析成功
********************************************************************************/
int parseHttpRequestMsgHead(char *line, HTTP_REQUEST_HEADER *pHead)
{
    int ret = SUCCESS;

    CHECK_POINT(line);
    CHECK_POINT(pHead);

    return ret;
}

int getMethed(char *method, HTTP_REQUEST_HEADER *pHead)
{
    int ret = SUCCESS;

    if (strcasecmp(method, "GET"))
    {
        pHead->method = GET;
        return ret;
    }
    else if (strcasecmp(method, "POST"))
    {
        pHead->method = POST;
        return ret;
    }
    else
    {
        pHead->method = METHOD_NOT_SUPPORT;
        ret = PARA_ERROR;
    }

    return ret;
}

int getVersion(char *version, HTTP_REQUEST_HEADER *pHead)
{
    int ret = SUCCESS;

    if (strcasecmp(version, "HTTP/1.0"))
    {
        pHead->version = HTTP_10;
        return ret;
    }
    else if (strcasecmp(version, "HTTP/1.1"))
    {
        pHead->version = HTTP_11;
        return ret;
    }
    else
    {
        pHead->version = VERSION_NOT_SUPPORT;
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
int parseHttpRequestMsgLine(char *line, HTTP_REQUEST_HEADER *pHead)
{
    int readNum = 0;
    int ret = SUCCESS;
    char word[3][MAX_LINE_LEN];

    CHECK_POINT(line);
    CHECK_POINT(pHead);

    memset(word, 0, sizeof(word));
    if (*line != '\0')
    {
        /* request method */
        readNum = splitStr(line, word, " ");
        LOG_INFO("method: %s", word[0]);
        ret = getMethed(word[0], pHead);

        /* request url */
        strncpy(pHead->url, word[1], MAX_LINE_LEN);

        /* request http version */
        ret = getVersion(word[2], pHead);
    }

    return ret;
}

/*******************************************************************************
 * @brief       解析获取的 http 报文内容，包括请求报文头和请求体
 * @param[in]   buf : buffer
 * @param[out]  http_data : http data struct
 * @note        
 * @return
 *              SUCCESS     解析成功
********************************************************************************/
int parseHttpData(char *buf, HTTP_REQUEST_DATA *http_data)
{
    int readNum = 0;
    int ret = SUCCESS;
    char line[MAX_LINE_LEN];
    HTTP_REQUEST_HEADER *pHead = NULL;

    CHECK_POINT(buf);
    CHECK_POINT(http_data);

    memset(http_data, 0, sizeof(HTTP_REQUEST_DATA));
    memset(line, 0, MAX_LINE_LEN);
    pHead = http_data->header;

    /* 解析报文内容 */
    while (*buf != '\0')
    {
        readNum = httpParseReadLine(buf, line, MAX_BUf_LEN, MAX_LINE_LEN);
        LOG_INFO("Read line: %s", line);
        buf += readNum;

        switch (http_data->state->parse_state)
        {
            case PARSE_REQUEST_LINE:
                ret = parseHttpRequestMsgLine(line, pHead);
                if (SUCCESS == ret) { http_data->state->parse_state = PARSE_REQUEST_HEAD; }
                break;
            case PARSE_REQUEST_HEAD:
                ret = parseHttpRequestMsgHead(line, pHead);
                break;
            case PARSE_REQUEST_BODY:
                break;
            default:
                break;
        }
        memset(line, 0, MAX_LINE_LEN);
    }

    LOG_INFO("[httpServer] method:%d, version:%d, url:%s", http_data->header->method, 
                                                            http_data->header->version, 
                                                            http_data->header->url);

finish:
    return ret;
}