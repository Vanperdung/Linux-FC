#pragma once

#include <cstdio>
#include <cstdarg>
#include <cstdint>
#include <functional>

class Logger
{
public:
    using LogFuncType = std::function<void(uint8_t *, uint16_t)>;

    static void printLog(const char *level, const char *fmt, ...);
    static void registerLogFunc(LogFuncType func);

private:
    static LogFuncType log_func;
};

#define LOG_ERROR_LEVEL (0)
#define LOG_WARNING_LEVEL (1)
#define LOG_INFO_LEVEL (2)
#define LOG_DEBUG_LEVEL (3)

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_INFO_LEVEL
#endif

#if LOG_LEVEL >= LOG_ERROR_LEVEL
#define LOG_ERROR(fmt, ...)                                  \
    do                                                       \
    {                                                        \
        Logger::printLog("ERROR", "[%s:%d]"                  \
                                  "\r\n" fmt,                \
                         __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define LOG_ERROR(fmt, ...) \
    do                      \
    {                       \
    } while (0)
#endif

#if LOG_LEVEL >= LOG_WARNING_LEVEL
#define LOG_WARNING(fmt, ...)                                \
    do                                                       \
    {                                                        \
        Logger::printLog("WARNING", "[%s:%d]"                \
                                    "\r\n" fmt,              \
                         __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define LOG_WARNING(fmt, ...) \
    do                        \
    {                         \
    } while (0)
#endif

#if LOG_LEVEL >= LOG_INFO_LEVEL
#define LOG_INFO(fmt, ...)                                   \
    do                                                       \
    {                                                        \
        Logger::printLog("INFO", "[%s:%d]"                   \
                                 "\r\n" fmt,                 \
                         __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define LOG_INFO(fmt, ...) \
    do                     \
    {                      \
    } while (0)
#endif

#if LOG_LEVEL >= LOG_DEBUG_LEVEL
#define LOG_DEBUG(fmt, ...)                                  \
    do                                                       \
    {                                                        \
        Logger::printLog("DEBUG", "[%s:%d]"                  \
                                  "\r\n" fmt,                \
                         __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define LOG_DEBUG(fmt, ...) \
    do                      \
    {                       \
    } while (0)
#endif