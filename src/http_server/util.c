/********************************************************************************
 * @file        util.c
 * @author      Fan6514
 * @version     V0.01
 * @date        26-SEP-2021
 * @brief       公共函数
 * @htmlonly
 * <span style="font-weight: bold">History</span> 
 * @endhtmlonly 
 * Version|Auther|Date|Describe
 * ------|----|------|-------- 
 * V0.1|Fan|26-SEP-2021|Create File
 * <h2><center>&copy;COPYRIGHT 2021 WELLCASA All Rights Reserved.</center></h2>
********************************************************************************/
#include <string.h>

#include "util.h"

/*******************************************************************************
 * @brief       读取缓冲区内的一行内容
 * @param[in]   buf : buffer
 * @param[out]  pLine : the first line in buf
 * @param[in]   maxBufSum : the max buf size
 * @param[in]   maxLineSum : the max line size
 * @note        输出参数 pLine 以 '\0' 结尾，忽略 '\n' 或 '\r\n'.
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

    while (*buf != '\0' && lineIndex < maxLineSum -1)
    {
        /* 忽略 \r */
        if (*buf == '\r') { buf += 2; break; }
        if (*buf == '\n') { buf++; break; }
        pLine[lineIndex] = *buf;
        buf++;
        lineIndex++;
    }
    pLine[lineIndex] = '\0';
    lineIndex++;

    return lineIndex;
}

/*******************************************************************************
 * @brief       分割字符串
 * @param[in]   line : buffer
 * @param[out]  word : words
 * @param[in]   delim : the splite char
 * @param[in]   maxOutNum : the nums of words
********************************************************************************/
void splitStr(char *line, char word[][MAX_LINE_LEN], const char delim, int maxOutNum)
{
    int wordIndex = 0;
    int strIndex = 0;
    char *subStr = NULL;
    char *lineTemp = NULL;

    CHECK_POINT_NORTN(line);
    CHECK_POINT_NORTN(word);

    while (*line == ' ')
    {
        line++;
    }

    lineTemp = line;
    while (*lineTemp != '\0' && wordIndex < maxOutNum-1)
    {
        if (*lineTemp == delim)
        {
            wordIndex++;
            lineTemp++;
            strIndex = 0;
            while (*lineTemp == ' ')
            {
                lineTemp++;
            }
            line = lineTemp;

            continue;
        }
        word[wordIndex][strIndex] = *lineTemp;
        strIndex++;
        lineTemp++;
    }
    strncpy(word[wordIndex], line, MAX_LINE_LEN);
}

BOOLEAN isSameStr(const char *src, const char *dist)
{
    return strcasecmp(src, dist) == 0 ? TRUE : FALSE;
}