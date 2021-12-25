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
int parseHttpRequestData(char *buf, HTTP_REQUEST_DATA **ppHttpRequestData)；
void httpRequestDataInit(HTTP_REQUEST_DATA **ppHttpRequestData);

#ifdef __cplusplus
}
#endif

#define HTTP_REQUEST "GET / HTTP1.0\r\n\
Accept:image/gif.image/jpeg.*/*\r\n\
Accept-Language:zh-cn\r\n\
Connection:Keep-Alive\r\n\Host:localhost\r\n\
User-Agent:Mozila/4.0(compatible:MSIE5.01:Windows NT5.0)\r\n\
Accept-Encoding:gzip,deflate.\r\n"

TEST_GROUP(utestHttpParse)
{
   void setup()
   {
   }

   void teardown()
   {
   }

   HTTP_REQUEST_DATA *constructHttpRequestData()
   {
      HTTP_REQUEST_DATA *pHttpRequestData = NULL;
      pHttpRequestData = (HTTP_REQUEST_DATA*)malloc(sizeof(HTTP_REQUEST_DATA));
      httpRequestDataInit(&pHttpRequestData);
      return pHttpRequestData;
   }
};



TEST(utestHttpParse, readRequestFirstLine)
{
   int ret = 0;
   char data[1024] = HTTP_REQUEST;
   char line[1024] = {0};
   ret = httpParseReadLine(data, line, 1024, 1024);
   STRCMP_EQUAL(line, "GET / HTTP1.0");
}

TEST(utestHttpParse, readRequestData)
{
   int ret = 0;
   char data[1024] = HTTP_REQUEST;
   HTTP_REQUEST_DATA *pHttpRequestData = NULL;

   pHttpRequestData = constructHttpRequestData();

   int ret = parseHttpRequestData(buf, &pHttpRequestData);
   CHECK_EQUAL(ret, SUCCESS);
}

