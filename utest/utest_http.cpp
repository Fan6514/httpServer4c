#include "CppUTest/UtestMacros.h"
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/TestHarness.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"
#include "url_reg.h"
#include "http_parse.h"
#include "http_server.h"
int httpParseReadLine(char *buf, char *pLine, int maxBufSum, int maxLineSum);
int parseHttpRequestData(char *buf, HTTP_REQUEST_DATA **ppHttpRequestData);
void httpRequestDataInit(HTTP_REQUEST_DATA **ppHttpRequestData);

#ifdef __cplusplus
}
#endif

#define HTTP_GET_REQUEST "GET /notfound HTTP/1.0\r\n\
Accept:image/gif.image/jpeg.*/*\r\n\
Accept-Language:zh-cn\r\n\
Connection:Keep-Alive\r\n\
Host:localhost\r\n\
User-Agent:Mozila/4.0(compatible:MSIE5.01:Windows NT5.0)\r\n\
Accept-Encoding:gzip,deflate.\r\n\r\n"

#define HTTP_POST_REQUEST "POST /notfound HTTP/1.0\r\n\
Accept:image/gif.image/jpeg.*/*\r\n\
Accept-Language:zh-cn\r\n\
Connection:Keep-Alive\r\n\
Host:localhost\r\n\
Content-Length: 30\r\n\
User-Agent:Mozila/4.0(compatible:MSIE5.01:Windows NT5.0)\r\n\
Accept-Encoding:gzip,deflate.\r\n\
\r\n\
username=admin&password=123123\r\n"

#define HTTP_NOTMETHOD_REQUEST "NOTMETHOD /notfound HTTP/1.0\r\n\
Accept:image/gif.image/jpeg.*/*\r\n\
Accept-Language:zh-cn\r\n\
Connection:Keep-Alive\r\n\
Host:localhost\r\n\
User-Agent:Mozila/4.0(compatible:MSIE5.01:Windows NT5.0)\r\n\
Accept-Encoding:gzip,deflate.\r\n\r\n"

#define HTTP_TEST_PAGE "GET /test HTTP/1.0\r\n\
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
    char data[1024] = HTTP_GET_REQUEST;
    char line[1024] = {0};
    ret = httpParseReadLine(data, line, 1024, 1024);
    STRCMP_EQUAL(line, "GET /notfound HTTP/1.0");
}

TEST(utestHttpParse, readGetRequestData)
{
    int ret = 0;
    char data[1024] = HTTP_GET_REQUEST;
    HTTP_REQUEST_DATA *pHttpRequestData = NULL;

    pHttpRequestData = (HTTP_REQUEST_DATA*)malloc(sizeof(HTTP_REQUEST_DATA));
    httpRequestDataInit(&pHttpRequestData);

    ret = parseHttpRequestData(data, &pHttpRequestData);
    CHECK_EQUAL(ret, SUCCESS);
    CHECK_EQUAL(pHttpRequestData->header->method, GET);
    CHECK_EQUAL(pHttpRequestData->header->version, HTTP_10);
    STRCMP_EQUAL(pHttpRequestData->header->url, "/notfound");
    STRCMP_EQUAL(pHttpRequestData->header->host, "localhost");
    CHECK_EQUAL(pHttpRequestData->header->keep_alive, TRUE);

    httpRequestDataUninit(&pHttpRequestData);
}

TEST(utestHttpParse, readPostRequestData)
{
    int ret = 0;
    char data[1024] = HTTP_POST_REQUEST;
    HTTP_REQUEST_DATA *pHttpRequestData = NULL;

    pHttpRequestData = (HTTP_REQUEST_DATA*)malloc(sizeof(HTTP_REQUEST_DATA));
    httpRequestDataInit(&pHttpRequestData);

    ret = parseHttpRequestData(data, &pHttpRequestData);
    CHECK_EQUAL(ret, SUCCESS);
    CHECK_EQUAL(pHttpRequestData->header->method, POST);
    CHECK_EQUAL(pHttpRequestData->header->version, HTTP_10);
    STRCMP_EQUAL(pHttpRequestData->header->url, "/notfound");
    STRCMP_EQUAL(pHttpRequestData->header->host, "localhost");
    STRCMP_EQUAL(pHttpRequestData->body, "username=admin&password=123123");
    CHECK_EQUAL(pHttpRequestData->header->keep_alive, TRUE);

    httpRequestDataUninit(&pHttpRequestData);
}

TEST_GROUP(utestHttpResponse)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(utestHttpResponse, httpRequestNotFound)
{
    int ret = 0;
    char data[1024] = HTTP_GET_REQUEST;
    HTTP_REQUEST_DATA *pHttpRequestData = NULL;
    HTTP_RESPONSE_DATA *pHttpResponseData = NULL;

    pHttpRequestData = (HTTP_REQUEST_DATA*)malloc(sizeof(HTTP_REQUEST_DATA));
    pHttpResponseData = (HTTP_RESPONSE_DATA*)malloc(sizeof(HTTP_RESPONSE_DATA));
    httpRequestDataInit(&pHttpRequestData);
    httpResponseDataInit(&pHttpResponseData);
    urlRegInit();

    ret = parseHttpRequestData(data, &pHttpRequestData);
    CHECK_EQUAL(ret, SUCCESS);

    ret = httpServerRequestHandler(pHttpRequestData, pHttpResponseData);
    CHECK_EQUAL(ret, SUCCESS);
    STRCMP_EQUAL(pHttpResponseData->header->rtncode, "404");

    httpRequestDataUninit(&pHttpRequestData);
    httpResponseDataUninit(&pHttpResponseData);
}

TEST(utestHttpResponse, httpRequestNotMethod)
{
    int ret = 0;
    char data[1024] = HTTP_NOTMETHOD_REQUEST;
    HTTP_REQUEST_DATA *pHttpRequestData = NULL;
    HTTP_RESPONSE_DATA *pHttpResponseData = NULL;

    pHttpRequestData = (HTTP_REQUEST_DATA*)malloc(sizeof(HTTP_REQUEST_DATA));
    pHttpResponseData = (HTTP_RESPONSE_DATA*)malloc(sizeof(HTTP_RESPONSE_DATA));
    httpRequestDataInit(&pHttpRequestData);
    httpResponseDataInit(&pHttpResponseData);
    urlRegInit();

    ret = parseHttpRequestData(data, &pHttpRequestData);
    CHECK_EQUAL(ret, SUCCESS);

    ret = httpServerRequestHandler(pHttpRequestData, pHttpResponseData);
    CHECK_EQUAL(ret, SUCCESS);
    STRCMP_EQUAL(pHttpResponseData->header->rtncode, "501");

    httpRequestDataUninit(&pHttpRequestData);
    httpResponseDataUninit(&pHttpResponseData);
}

TEST(utestHttpResponse, httpRequestPostNotFound)
{
    int ret = 0;
    char data[1024] = HTTP_POST_REQUEST;
    HTTP_REQUEST_DATA *pHttpRequestData = NULL;
    HTTP_RESPONSE_DATA *pHttpResponseData = NULL;

    pHttpRequestData = (HTTP_REQUEST_DATA*)malloc(sizeof(HTTP_REQUEST_DATA));
    pHttpResponseData = (HTTP_RESPONSE_DATA*)malloc(sizeof(HTTP_RESPONSE_DATA));
    httpRequestDataInit(&pHttpRequestData);
    httpResponseDataInit(&pHttpResponseData);
    urlRegInit();

    ret = parseHttpRequestData(data, &pHttpRequestData);
    CHECK_EQUAL(ret, SUCCESS);

    ret = httpServerRequestHandler(pHttpRequestData, pHttpResponseData);
    CHECK_EQUAL(ret, SUCCESS);
    STRCMP_EQUAL(pHttpResponseData->header->rtncode, "404");

    httpRequestDataUninit(&pHttpRequestData);
    httpResponseDataUninit(&pHttpResponseData);
}

TEST(utestHttpResponse, httpRequestGetTestPage)
{
    int ret = 0;
    char data[1024] = HTTP_TEST_PAGE;
    HTTP_REQUEST_DATA *pHttpRequestData = NULL;
    HTTP_RESPONSE_DATA *pHttpResponseData = NULL;

    pHttpRequestData = (HTTP_REQUEST_DATA*)malloc(sizeof(HTTP_REQUEST_DATA));
    pHttpResponseData = (HTTP_RESPONSE_DATA*)malloc(sizeof(HTTP_RESPONSE_DATA));
    httpRequestDataInit(&pHttpRequestData);
    httpResponseDataInit(&pHttpResponseData);
    urlRegInit();

    ret = parseHttpRequestData(data, &pHttpRequestData);
    CHECK_EQUAL(ret, SUCCESS);

    ret = httpServerRequestHandler(pHttpRequestData, pHttpResponseData);
    CHECK_EQUAL(ret, SUCCESS);
    STRCMP_EQUAL(pHttpResponseData->header->rtncode, "200");

    httpRequestDataUninit(&pHttpRequestData);
    httpResponseDataUninit(&pHttpResponseData);
}
