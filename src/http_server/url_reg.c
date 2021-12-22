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

URL_PROC_TYPE urls[] = {
    {ROOT_URL, ROOT_URL_STR, procRootUrl}
};

/*******************************************************************************
 * @brief       处理 url 入口函数
 * @param[in]   buf : buffer
 * @param[out]  pLine : the first line in buf
 * @param[in]   maxBufSum : the max buf size
 * @param[in]   maxLineSum : the max line size
 * @note        输出参数 pLine 以 '\0' 结尾，包含 '\n'.
 * @return
 *              PARA_ERROR  入参错误
 *              lineIndex   读取行的字符数
********************************************************************************/
void procUrl(int urlID)
{
    
}
