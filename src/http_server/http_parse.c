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

int parseHttpRequestHead(char *head_buf, HTTP_REQUEST_DATA *http_data)
{
    int ret = SUCCESS;

    CHECK_POINT(head_buf);
    CHECK_POINT(http_data);

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
    int nPos = 0;
    int ret = SUCCESS;
    char *head_buf = NULL;
    char line[MAX_LINE_LEN];

    CHECK_POINT(buf);
    CHECK_POINT(http_data);

    memset(http_data, 0, sizeof(HTTP_REQUEST_DATA));
    memset(line, 0, MAX_LINE_LEN);
    GET_MEMORY(head_buf, char, MAX_HTTP_HEAD_LEN, finish);

    /* 获取报文头 */
    while (0 != strncmp(line, "\r\n"))
    {
        if (temp_buf == strstr(temp_buf, "\r\n\r\n"))
        {
            break;
        }
        nPos++;
        temp_buf++;
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