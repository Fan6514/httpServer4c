#include "CppUTest/UtestMacros.h"
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/TestHarness.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"
#include "http_parse.h"
#include "http_server.h"
int httpParseReadLine(char *buf, char *pLine, int maxBufSum, int maxLineSum);
int parseHttpRequestData(char *buf, HTTP_REQUEST_DATA **ppHttpRequestData);
void httpRequestDataInit(HTTP_REQUEST_DATA **ppHttpRequestData);

#ifdef __cplusplus
}
#endif

#define HTTP_REQUEST "GET / HTTP/1.0\r\n\
Accept:image/gif.image/jpeg.*/*\r\n\
Accept-Language:zh-cn\r\n\
Connection:Keep-Alive\r\n\
Host:localhost\r\n\
User-Agent:Mozila/4.0(compatible:MSIE5.01:Windows NT5.0)\r\n\
Accept-Encoding:gzip,deflate.\r\n\r\n"

TEST_GROUP(utestHttpParse)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(utestHttpParse, readRequestFirstLine)
{
    int ret = 0;
    char data[1024] = HTTP_REQUEST;
    char line[1024] = {0};
    ret = httpParseReadLine(data, line, 1024, 1024);
    STRCMP_EQUAL(line, "GET / HTTP/1.0");
}

TEST(utestHttpParse, readRequestData)
{
    int ret = 0;
    char data[1024] = HTTP_REQUEST;
    HTTP_REQUEST_DATA *pHttpRequestData = NULL;

    pHttpRequestData = (HTTP_REQUEST_DATA*)malloc(sizeof(HTTP_REQUEST_DATA));
    httpRequestDataInit(&pHttpRequestData);

    ret = parseHttpRequestData(data, &pHttpRequestData);
    CHECK_EQUAL(ret, SUCCESS);
    CHECK_EQUAL(pHttpRequestData->header->method, GET);
    CHECK_EQUAL(pHttpRequestData->header->version, HTTP_10);
    STRCMP_EQUAL(pHttpRequestData->header->url, "/");
    STRCMP_EQUAL(pHttpRequestData->header->host, "localhost");
    CHECK_EQUAL(pHttpRequestData->header->keep_alive, TRUE);

    httpRequestDataUninit(&pHttpRequestData);
}

