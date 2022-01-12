# httpServer4c
本项目使用 C 语言实现 HTTP 服务器，能够实现基本的 GET 和 POST 请求，并将处理结果发送给浏览器显示。HTTP 服务器采用 Linux epoll 模型，并手写了一个简易的线程池来处理每一条链接，实现 one loop per thread + thread pool 模式 。

## 系统结构

HTTP 协议的底层使用 TCP 协议，HTTP/1.0 版本默认使用短链接，即浏览器和服务器每进行一次 HTTP 操作，就建立一次连接，任务结束以后中断连接。 

整个系统的框架如下：

![httpstruct](pics\httpmodel.png)

服务器收到浏览器发送的 HTTP 请求后建立 TCP 连接，由线程池分配线程进行相应的请求处理。

Controller 是 HTTP 连接的控制层，负责将 HTTP 请求报文解析为内部数据结构，根据 url 调用相应的视图函数处理请求，然后将处理结果组成 HTTP 响应报文发送给连接的浏览器。

Views 是 HTTP 连接的视图层，负责处理具体的 HTTP 请求，将响应的页面信息等填充到  HTTP 响应结构。

## 日志系统

除基础C库外，只有日志系统使用了 log4c 库。

log4c 是基本是纯C语言写成的，相比 log4cpp、log4cxx 等日志系统具有很好的移植性。

**1. 基本概念**

 Log4c 中有三个重要的概念：Category, Appender, Layout。

Category 用于区分不同的 Logger，其实它就是个 Logger。在一个程序中我们可以通过 Category 来指定很多的 Logger，用于不同的目的。

Appdender 用于**描述输出流**，通过为 Category 来指定一个 Appdender，可以决定将 log 信息来输出到什么地方去，比如stdout、stderr、文件, 或者是 socket 等等。

Layout 用于**指定日志信息的格式**，通过为 Appender 来指定一个 Layout ，可以决定 log 信息以何种格式来输出，比如是否有带有时间戳， 是否包含文件位置信息等，以及他们在一条log信息中的输出格式等。 

**2. 安装方法**

在官网中下载源码：[log4c-1.2.4.tar.gz](http://prdownloads.sourceforge.net/log4c/log4c-1.2.4.tar.gz)

 log4c 包使用 GNU autotools 编译和安装框架，使用以下命令进行安装：

```shell
tar -zxvf log4c-1.2.4.tar.gz
cd log4c-1.2.4/
./configure --prefix=/usr/log4c
make
make install
```

此时我们将 log4c 安装到 /usr/log4c 目录下，在编译项目是需要连接库和头文件：

```makefile
-I /usr/log4c/include
-L /usr/log4c/lib
```

**3. log4c的使用方法**

在 log 函数的同级目录中添加配置文件 log4crc：

```xml
<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE log4c SYSTEM "">

<log4c version="1.2.4">

		<config>
                <bufsize>0</bufsize>
                <debug level="0"/>
                <nocleanup>0</nocleanup>
        </config>
        <!-- root category ========================================= -->
        <category name="root" priority="trace" appender="stdout"/>
        <!-- httpserver category ========================================= -->
        <category name="httpserver" priority="trace" appender="stdout"/>
        <!-- default appenders ===================================== -->
        <appender name="stdout" type="stream" layout="basic"/>
        <appender name="stderr" type="stream" layout="dated"/>
        <appender name="syslog" type="syslog" layout="dated" logdir="./" prefix="testLog"/>
        <!-- default layouts ======================================= -->
        <layout name="basic" type="basic"/>
        <layout name="dated" type="dated"/>
</log4c>
```

创建 logger 函数进行初始化：

```c
log4c_category_t* loggerCategory = NULL;

void loggerInit(const char* categoryName)
{
    if(log4c_init() != 0)
    {
        printf("log4c init error.");
        return;
    }

    loggerCategory = log4c_category_get(categoryName);
}

void loggerUninit()
{
    if(log4c_fini() != 0)
    {
        printf("log4c uninit error.");
    }
}
```

其中，categoryName 为配置中 category 对应的名称。

为了方便在项目中使用，我们对日志函数进行了一定的封装：

```c
extern log4c_category_t* loggerCategory;

#define LOG_ERROR(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_ERROR, msg, ##args); \
}while(0)

#define LOG_WARN(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_WARN, msg, ##args); \
}while(0)

#define LOG_INFO(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_INFO, msg, ##args); \
}while(0)

#define LOG_TRACE(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_TRACE, msg, ##args); \
}while(0)

#define LOG_DEBUG(msg, args...)\
do\
{\
    const log4c_location_info_t locinfo = LOG4C_LOCATION_INFO_INITIALIZER(NULL);\
    log4c_category_log_locinfo(loggerCategory, &locinfo, LOG4C_PRIORITY_DEBUG, msg, ##args); \
}while(0)
```

## 单元测试

在项目编码过程中，使用了 CPPUTest 框架进行单元测试。

**1. 安装方法**

在官网下载源码： http://cpputest.github.io/ 

```shell
tar -zxvf cpputest-3.6.tar.gz
cd cpputest-3.6/
./configure --prefix=/usr
make
```

**2. 使用方法**

```c
// utest_xxx.cpp
#include "CppUTest/UtestMacros.h"
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/TestHarness.h>

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
}

// main.cpp
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestPlugin.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupportPlugin.h"

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
```

**3. 编译方法**

在编译过程中，由于源码中使用了线程库和 log4c 库，所以在编译过程中要在 MakefileWorker.mk 文件中链接这两个库，并且添加 log4c 库的头文件地址。

```makefile
## CppUTest Makefile
#
##----------

#--- Inputs ----#
# Inputs - these variables describe what to build
COMPONENT_NAME = UTEST_HTTP_SERVER
CPPUTEST_HOME = /usr
PROJECT_DIR = {cd ../ && pwd}

CPP_PLATFORM = Gcc

# This line is overriding the default new macros.  This is helpful
# when using std library includes like <list> and other containers
# so that memory leak detection does not conflict with stl.
SRC_DIRS = \
        ${PROJECT_DIR}/src\
        ${PROJECT_DIR}/src/http_server\
        ${PROJECT_DIR}/src/log\
        ${PROJECT_DIR}/src/thread_pool

TEST_SRC_DIRS = \
        ${PROJECT_DIR}/utest

INCLUDE_DIRS =\
        ${PROJECT_DIR}/src/include\
        /usr/log4c/include

CPPUTEST_LIB_DIR = /usr/log4c/lib

CPPUTEST_WARNINGFLAGS = -Wall -std=gnu99

CPPUTEST_USE_GCOV = Y

#  Other flags users can initialize to sneak in their settings
LD_LIBRARIES = -lpthread -llog4c
LIB_DIRS = /usr/log4c/lib

include MakefileWorker.mk
```

