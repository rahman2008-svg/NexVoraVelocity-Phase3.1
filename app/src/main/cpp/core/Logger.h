#pragma once

#include <android/log.h>
#include <string>
#include <cstdarg>

#define NV_LOG_TAG "NexVoraVelocity"

namespace nexvora {

enum class LogLevel {
    Verbose = 0,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};

class Logger {
public:
    static void log(LogLevel level, const char* tag, const char* fmt, ...);
    static void v(const char* fmt, ...);
    static void d(const char* fmt, ...);
    static void i(const char* fmt, ...);
    static void w(const char* fmt, ...);
    static void e(const char* fmt, ...);
    static void f(const char* fmt, ...);

private:
    static int toAndroidPriority(LogLevel level);
};

} // namespace nexvora

#define NV_LOGV(...) ::nexvora::Logger::v(__VA_ARGS__)
#define NV_LOGD(...) ::nexvora::Logger::d(__VA_ARGS__)
#define NV_LOGI(...) ::nexvora::Logger::i(__VA_ARGS__)
#define NV_LOGW(...) ::nexvora::Logger::w(__VA_ARGS__)
#define NV_LOGE(...) ::nexvora::Logger::e(__VA_ARGS__)
#define NV_LOGF(...) ::nexvora::Logger::f(__VA_ARGS__)
