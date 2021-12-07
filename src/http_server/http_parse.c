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

#include "http_parse.h"
#include "util.h"

int httpParseSplitWord

/*******************************************************************************
 * @brief       读取缓冲区内的一行内容
 * @param[in]   buf : buffer
 * @param[out]  pLine : the first line in buf
 * @param[in]   maxBufSum : the max buf size
 * @param[in]   maxLineSum : the max line size
 * @note        输出参数 pLine 以 '\0' 结尾，不包含 '\n'.
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

    while (*buf != '\0' && *buf != '\n' && lineIndex < maxLineSum)
    {
        pLine[lineIndex] = *buf;
        lineIndex++;
        buf++;
    }
    pLine[lineIndex] = '\0';
    
    return lineIndex;
}

int splitWordBlack(char *line, char *word)
{
    int wordIndex = 0;

    CHECK_POINT(buf);
    CHECK_POINT(line);

    /* 跳过字符串开头的空行和'\n' */
    while(*line == ' ' || *line == '\n')
    {
        buf++;
    }

    while (*line != ' ' && *line != '\0' && *line != '\n')
    {
        pLine[wordIndex] = *buf;
        wordIndex++;
        buf++;
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
int parseHttpRequestHead(char *head_buf, HTTP_REQUEST_DATA *http_data)
{
    int ret = SUCCESS;

    CHECK_POINT(head_buf);
    CHECK_POINT(http_data);

    return ret;
}

int getMethed(char *method, HTTP_REQUEST_DATA *http_data)
{
    int ret = SUCCESS;
    HTTP_REQUEST_HEADER *pHead = NULL;
    pHead = http_data->header;

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

int getVersion(char *version, HTTP_REQUEST_DATA *http_data)
{
    int ret = SUCCESS;
    HTTP_REQUEST_HEADER *pHead = NULL;
    pHead = http_data->header;

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
 * @param[in]   line : buffer
 * @param[out]  http_data : http data struct
 * @note        
 * @return
 *              SUCCESS     解析成功
********************************************************************************/
int parseHttpRequestMsgLine(char *line, HTTP_REQUEST_DATA *http_data)
{
    int readNum = 0;
    int ret = SUCCESS;
    HTTP_REQUEST_HEADER *pHead = NULL;
    char word[MAX_LINE_LEN];

    CHECK_POINT(line);
    CHECK_POINT(http_data);

    pHead = http_data->header;
    memset(word, 0, sizeof(word));
    if (*line != '\0')
    {
        /* request method */
        readNum = splitWordBlack(line, word);
        if (readNum <= 0) { break; }
        ret = getMethed(word, http_data);
        line += readNum;
        memset(word, 0, sizeof(word));

        /* request url */
        readNum = splitWordBlack(line, word);
        if (readNum <= 0) { break; }
        strncpy(pHead->url, word, MAX_LINE_LEN);
        line += readNum;
        memset(word, 0, sizeof(word));

        /* request http version */
        readNum = splitWordBlack(line, word);
        if (readNum <= 0) { break; }
        ret = getVersion(word, http_data);
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
    char *head_buf = NULL;
    char line[MAX_LINE_LEN];

    CHECK_POINT(buf);
    CHECK_POINT(http_data);

    memset(http_data, 0, sizeof(HTTP_REQUEST_DATA));
    memset(line, 0, MAX_LINE_LEN);
    GET_MEMORY(head_buf, char, MAX_HTTP_HEAD_LEN, finish);

    /* 获取报文头 */
    while (*buf != "\0")
    {
        readNum = httpParseReadLine(buf, line, MAX_BUf_LEN, MAX_LINE_LEN);
        if (readNum <= 0) { break; }

        buf += readNum;
    }
    strncpy(head_buf, buf, nPos);
    LOG_DEBUG("[httpServer] head_buf:%s", head_buf);

    ret = parseHttpRequestHead(head_buf, http_data);
    CHECK_RETURN_GOTO(ret, SUCCESS, finish, "[httpServer] parseHttpRequestHead error.");
    LOG_INFO("[httpServer] method:%d, version:%d, url:%s", http_data->header.method, http_data->header.version, 
                                                            http_data->header.url);

finish:
    REL_MEMORY(head_buf);
    return ret;
}