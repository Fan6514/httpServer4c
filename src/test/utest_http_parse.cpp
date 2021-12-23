#include "CppUTest/UtestMacros.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/http_parse.h"
int httpParseReadLine(char *buf, char *pLine, int maxBufSum, int maxLineSum);

#ifdef __cplusplus
}
#endif

TEST_GROUP(utestHttpParse)
{
};

TEST(utestHttpParse, readRequestFirstLine)
{
   int ret = 0;
   char buf[1024] = "get / http1.0";
   char line[1024] = {0};
   ret = httpParseReadLine(buf, line, 1024, 1024);
   CHECK_EQUAL(0, ret);
}

