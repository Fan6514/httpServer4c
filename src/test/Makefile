CC=gcc
CXX=g++

#自己定义这个路径
CPPUTEST_HOME=/usr
INCLUDE = -I ./include -I /usr/log4c/include

FLAGS= -c

CPPFLAGS += -I$(CPPUTEST_HOME)/include

CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h

LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

#自己定义最后程序的名称
TARGET=UTEST_HTTP

#自己定义有哪些文件
OBJECTS=main.o utest_http_parse.o
SOURCES=main.cpp utest_http_parse.cpp

$(TARGET):$(OBJECTS)
	$(CXX) $^ $(LD_LIBRARIES) -o $@


.cpp.o:
	$(CXX) $(FLAGS) $(CPPFLAGS) ${INCLUDE} $(CXXFLAGS) $< -o $@

.c.o:
	$(CXX) $(FLAGS) $(CPPFLAGS) ${INCLUDE} $(CFLAGS) $< -o $@

clean:
	rm *.o $(TARGET)