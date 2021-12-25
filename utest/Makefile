#---------
#
## CppUTest Makefile
#
##----------

#Set this to @ to keep the makefile quiet
ifndef SILENCE
        SILENCE = @
endif

#--- Inputs ----#
# Inputs - these variables describe what to build
COMPONENT_NAME = UTEST_HTTP_SERVER
CPPUTEST_HOME = /usr

CPPUTEST_USE_EXTENSIONS = Y
CPP_PLATFORM = Gcc

# This line is overriding the default new macros.  This is helpful
# when using std library includes like <list> and other containers
# so that memory leak detection does not conflict with stl.
SRC_DIRS = \
        /root/httpServer4c/src

TEST_SRC_DIRS = \
        /root/httpServer4c/utest

INCLUDE_DIRS =\
        /root/httpServer4c/src/include\
        /usr/log4c/include

CPPUTEST_LIB_DIR = /usr/log4c/lib

CPPUTEST_USE_STD_CPP_LIB = -std=c99
CPPUTEST_WARNINGFLAGS = -Wall

CPPUTEST_USE_GCOV = Y

#  Other flags users can initialize to sneak in their settings
LDFLAGS = -llog4c -lpthread


include MakefileWorker.mk