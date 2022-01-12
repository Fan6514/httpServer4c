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