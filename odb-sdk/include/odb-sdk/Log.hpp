#pragma once

#include "odb-sdk/config.hpp"
#include <cstdio>

namespace odb {
namespace log {

enum Severity
{
    INFO,
    NOTICE,
    WARNING,
    ERROR,
    FATAL
};

ODBSDK_PUBLIC_API void init();
ODBSDK_PUBLIC_API void deinit();
ODBSDK_PUBLIC_API void setInfoLog(FILE* fp);
ODBSDK_PUBLIC_API void setDataLog(FILE* fp);

ODBSDK_PUBLIC_API void info(const char* fmt, ...);
ODBSDK_PUBLIC_API void data(const char* fmt, ...);

ODBSDK_PUBLIC_API void dbParser(Severity severity, const char* fmt, ...);
ODBSDK_PUBLIC_API void vdbParser(Severity severity, const char* fmt, va_list ap);
ODBSDK_PUBLIC_API void cmd(Severity severity, const char* fmt, ...);
ODBSDK_PUBLIC_API void vcmd(Severity severity, const char* fmt, va_list ap);
ODBSDK_PUBLIC_API void ast(Severity severity, const char* fmt, ...);
ODBSDK_PUBLIC_API void sdk(Severity severity, const char* fmt, ...);
ODBSDK_PUBLIC_API void vsdk(Severity severity, const char* fmt, va_list ap);

}
}
