#include "Logger.h"
#include <cstdio>

namespace nexvora {

int Logger::toAndroidPriority(LogLevel level) {
    switch (level) {
        case LogLevel::Verbose: return ANDROID_LOG_VERBOSE;
        case LogLevel::Debug:   return ANDROID_LOG_DEBUG;
        case LogLevel::Info:    return ANDROID_LOG_INFO;
        case LogLevel::Warn:    return ANDROID_LOG_WARN;
        case LogLevel::Error:   return ANDROID_LOG_ERROR;
        case LogLevel::Fatal:   return ANDROID_LOG_FATAL;
        default:                return ANDROID_LOG_INFO;
    }
}

void Logger::log(LogLevel level, const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(toAndroidPriority(level), tag, fmt, args);
    va_end(args);
}

void Logger::v(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_VERBOSE, NV_LOG_TAG, fmt, args);
    va_end(args);
}

void Logger::d(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_DEBUG, NV_LOG_TAG, fmt, args);
    va_end(args);
}

void Logger::i(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_INFO, NV_LOG_TAG, fmt, args);
    va_end(args);
}

void Logger::w(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_WARN, NV_LOG_TAG, fmt, args);
    va_end(args);
}

void Logger::e(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_ERROR, NV_LOG_TAG, fmt, args);
    va_end(args);
}

void Logger::f(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(ANDROID_LOG_FATAL, NV_LOG_TAG, fmt, args);
    va_end(args);
}

} // namespace nexvora
