#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "cJSON.h"

#include "epoll.h"
#include "socket.h"
#include "thread_pool.h"
#include "http_server.h"
#include "util.h"
#include "url_reg.h"

void parseServerConfig(cJSON *serverJson, CONFIG_INFO *pConfigInfo)
{
    CHECK_POINT_NORTN(serverJson);
    CHECK_POINT_NORTN(pConfigInfo);

    pConfigInfo->port = cJSON_GetObjectItem(serverJson,"port")->valueint;
    pConfigInfo->poolSize = cJSON_GetObjectItem(serverJson,"thread_pool_size")->valueint;
    pConfigInfo->poolCoreSize = cJSON_GetObjectItem(serverJson,"thread_pool_core_size")->valueint;
}

void parseSqlConfig(cJSON *sqlJson, CONFIG_INFO *pConfigInfo)
{
    char *pValue = NULL;

    CHECK_POINT_NORTN(serverJson);
    CHECK_POINT_NORTN(pConfigInfo);

    pConfigInfo->sqlPoolSize = cJSON_GetObjectItem(sqlJson,"pool_size")->valueint;
    pValue = cJSON_GetObjectItem(sqlJson,"user")->valuestring;
    strncpy(pConfigInfo->sqlUser, pValue, sizeof(pConfigInfo->sqlUser));
    pValue = cJSON_GetObjectItem(sqlJson,"password")->valuestring;
    strncpy(pConfigInfo->sqlPasswd, pValue, sizeof(pConfigInfo->sqlPasswd));
}

void parseLogConfig(cJSON *logJson, CONFIG_INFO *pConfigInfo)
{
    char *pValue = NULL;

    CHECK_POINT_NORTN(serverJson);
    CHECK_POINT_NORTN(pConfigInfo);

    pValue = cJSON_GetObjectItem(serverJson,"category_name")->valuestring;
    strncpy(pConfigInfo->logCategoryName, pValue, sizeof(pConfigInfo->logCategoryName));
}

void parseJson(CONFIG_INFO *pConfigInfo, char *pBuf, int bufLen)
{
    cJSON *mainJson = NULL;
    cJSON *serverJson = NULL;
    cJSON *sqlJson = NULL;

    CHECK_POINT_NORTN(pConfigInfo);
    CHECK_POINT_NORTN(pBuf);

    mainJson = cJSON_Parse(pBuf);
    CHECK_EXPRESSION_NORTN(NULL == mainJson, "Parse json error.");

    serverJson = cJSON_GetObjectItem(mainJson, "server");
    parseServerConfig(serverJson, pConfigInfo);
    sqlJson = cJSON_GetObjectItem(mainJson, "sql");
    parseSqlConfig(sqlJson, pConfigInfo);
    logJson = cJSON_GetObjectItem(mainJson, "log");
    parseLogConfig(logJson, pConfigInfo);
}

void configSet(CONFIG_INFO *pConfigInfo)
{
    long fileLen = 0;
    char fileName[MAX_FILE_NAME_LEN] = {0};
    FILE *fp = NULL;
    char *pBuf = NULL;

    CHECK_POINT_NORTN(pConfigInfo);

    getcwd(fileName, MAX_FILE_NAME_LEN);
    strncat(fileName, "config.json", MAX_FILE_NAME_LEN);
    fp = fopen(fileName, "r");
    CHECK_EXPRESSION_NORTN(NULL == fp, "Not found config file %s.\n", fileName);

    /* 获取配置内容 */
    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    pBuf = (char*)malloc(fileLen+1);
    fread(pBuf, fileLen, 1, fp);

    parseJson(pConfigInfo, pBuf, fileLen+1);
}

void configInitDefault(CONFIG_INFO *pConfigInfo)
{
    CHECK_POINT(pConfigInfo);

    pConfigInfo->port = 8000;
    pConfigInfo->poolSize = 100;
    pConfigInfo->poolCoreSize = 10;

    strncpy(pConfigInfo->logCategoryName, "httpserver", sizeof(pConfigInfo->logCategoryName));
}

void showHelp()
{
    printf(SHOW_HELP);
}

int argParse(int argc, char *argv[], CONFIG_INFO *pConfigInfo)
{
    int opt = 0;
    const char *optStr = "chp:"

    CHECK_POINT(pConfigInfo);

    configInitDefault(pConfigInfo);

    while ((opt = getopt(argc, argv, optStr)) != -1)
    {
        switch (opt)
        {
            case 'c':
                configSet(pConfigInfo);
                break;
            case 'h':
                showHelp();
                break;
            case 'p':
                pConfigInfo->port = atoi(optarg);
                break;
            default:
                printf("Error para: %c.\n", opt);
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    CONFIG_INFO tConfigInfo = {0};

    /* 命令行解析 */
    argParse(argc, argv, &tConfigInfo);

    /* 初始化日志 */
    loggerInit(tConfigInfo.logCategoryName);
    /* 注册url */
    urlRegInit();
    /* 连接sql */
    sqlConnect();
    /* 服务器运行 */
    httpServerRun(&tConfigInfo);

    loggerUninit();
    return 0;
}