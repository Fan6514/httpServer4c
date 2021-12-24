#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestPlugin.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupportPlugin.h"

int main(int argc, char **argv)
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}