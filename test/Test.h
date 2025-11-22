#pragma once

#include <utils/Logger.h>
#include <utils/Guard.h>

class Test
{
public:
    Test() = default;
    virtual ~Test() = default;

    virtual bool init() { return true; }
    virtual bool run() { return true; }
    virtual bool cleanup() { return true; }
};

#define REGISTER_TEST(class)                                     \
    int main(int argc, char *argv[])                             \
    {                                                            \
        Test *p = new class();                                   \
        CHECK_PRINT_GOTO(p->init() != true, error, "FAILED");    \
        CHECK_PRINT_GOTO(p->run() != true, error, "FAILED");     \
        CHECK_PRINT_GOTO(p->cleanup() != true, error, "FAILED"); \
        LOG_INFO("PASSED");                                      \
    error:                                                       \
        delete p;                                                \
        return 0;                                                \
    }
