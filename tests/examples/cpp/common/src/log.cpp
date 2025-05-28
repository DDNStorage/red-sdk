/******************************************************************************
 *
 * @file
 * @copyright
 *                               --- WARNING ---
 *
 *     This work contains trade secrets of DataDirect Networks, Inc.  Any
 *     unauthorized use or disclosure of the work, or any part thereof, is
 *     strictly prohibited. Any use of this work without an express license
 *     or permission is in violation of applicable laws.
 *
 * @copyright DataDirect Networks, Inc. CONFIDENTIAL AND PROPRIETARY
 * @copyright DataDirect Networks Copyright, Inc. (c) 2021-2025. All rights reserved.
 *
 * @section DESCRIPTION
 *
 *   Name:       log.cpp
 *   Project:    Red SDK examples common library
 *
 *   Description: Common log functionality
 *
 ******************************************************************************/

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <ctime>
#include <sys/time.h>

#include "../include/log.hpp"

constexpr size_t MAX_LOG_FORMAT_STR = 4096;
constexpr size_t MAX_TIME_LEN       = 16;

namespace
{
int count_format_specifiers(const char *fmt)
{
    int         count = 0;
    const char *pTmp  = fmt;
    while ((pTmp = strchr(pTmp, '%')) != NULL)
    {
        pTmp++;
        count++;
    }
    return count;
}
} // namespace

void common::common_log(char const *file,
                        const int   line,
                        const char *func,
                        const char *fmt,
                        ...)
{
    if (fmt == NULL)
        return;

    int            time_len = 0;
    struct tm      tm_info;
    struct timeval tv;
    char           time_buffer[MAX_TIME_LEN];

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm_info);
    time_len = strftime(time_buffer, MAX_TIME_LEN, "%H:%M:%S", &tm_info);
    snprintf(time_buffer + time_len, MAX_TIME_LEN - time_len, ".%03ld",
             tv.tv_usec / 1000);

    int fmt_specifiers = count_format_specifiers(fmt);
    if (fmt_specifiers > 0)
    {
        char buf[MAX_LOG_FORMAT_STR];

        va_list callingArgs;
        va_start(callingArgs, fmt);
        int len = vsnprintf(buf, MAX_LOG_FORMAT_STR - 1, fmt, callingArgs);
        va_end(callingArgs);

        if (len > 0)
        {
            printf("%s - %s:%d %s: %s\n", time_buffer, file, line, func, buf);
        }
    }
    else
    {
        printf("%s - %s:%d %s: %s\n", time_buffer, file, line, func, fmt);
    }

    fflush(stdout);
}
