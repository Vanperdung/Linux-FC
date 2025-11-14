#pragma once

#include "Types.h"
#include "Logger.h"

#define CHECK_RET(expr, ret) \
    do                       \
    {                        \
        if ((expr))          \
        {                    \
            return ret;      \
        }                    \
    } while (0)

#define CHECK_PRINT_RET(expr, ret, fmt, ...) \
    do                                       \
    {                                        \
        if ((expr))                          \
        {                                    \
            LOG_ERROR(fmt, ##__VA_ARGS__);   \
            return ret;                      \
        }                                    \
    } while (0)

#define CHECK_PRINT(expr, fmt, ...)        \
    do                                     \
    {                                      \
        if ((expr))                        \
        {                                  \
            LOG_ERROR(fmt, ##__VA_ARGS__); \
        }                                  \
    } while (0)
