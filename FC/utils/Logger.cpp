#include "Logger.h"

using namespace FC;

Logger::LogFuncType Logger::logFunc = nullptr;

void Logger::printLog(const char *level, const char *fmt, ...)
{
    char buffer[512] = {0};
    int n = snprintf(buffer, sizeof(buffer), "[%s] ", level);

    va_list args;
    va_start(args, fmt);
    n += vsnprintf(buffer + n, sizeof(buffer) - n, fmt, args);
    va_end(args);

    if (logFunc)
        logFunc(reinterpret_cast<uint8_t *>(buffer), static_cast<uint16_t>(n));
}

void Logger::registerLogFunc(Logger::LogFuncType func)
{
    logFunc = func;
}