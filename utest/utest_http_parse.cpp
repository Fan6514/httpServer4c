#include "CppUTest/UtestMacros.h"
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/TestHarness.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "http_parse.h"
extern int httpParseReadLine(char *buf, char *pLine, int maxBufSum, int maxLineSum);

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
};

TEST(utestHttpParse, readRequestFirstLine)
{
   int ret = 0;
   char data[1024] = HTTP_REQUEST;
   char line[1024] = {0};
   ret = httpParseReadLine(buf, line, 1024, 1024);
   CHECK_EQUAL(0, ret);
   STRCMP_EQUAL(line, "GET / HTTP1.0\r\n");
}

