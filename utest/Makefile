#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

#---- Outputs ----#
COMPONENT_NAME = CppUTestExt

#--- Inputs ----#
CPPUTEST_HOME = .
CPP_PLATFORM = Gcc

WARNINGFLAGS = -pedantic-errors -Wall -Wextra -Werror -Wshadow -Wswitch-default -Wswitch-enum -Wconversion

SRC_DIRS = ../src/http_server
	
TEST_SRC_DIRS = ./

INCLUDE_DIRS = ../src/include

LD_LIBRARIES += -lstdc++ -L$(CPPUTEST_LIB_DIR)/$(TARGET_PLATFORM) -lCppUTest

include $(CPPUTEST_HOME)/build/MakefileWorker.mk