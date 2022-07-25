#include "error_functions.h"
#define MAX_LENGTH 1024

void output_err(bool error_flag, int error, bool flush_stdout, const char *prefix, const char *fmt, va_list args)
{
    char buf[MAX_LENGTH];
    snprintf(buf, MAX_LENGTH - 1, "%s: ", prefix);
    vsnprintf(buf + strlen(buf), MAX_LENGTH - strlen(buf) - 1, fmt, args);

    if (error_flag)
        snprintf(buf + strlen(buf), MAX_LENGTH - strlen(buf) - 1, "[%s]", strerror(error));

    snprintf(buf + strlen(buf), MAX_LENGTH - strlen(buf) - 1, "\n");

    if (flush_stdout)
        fflush(stdout);
    fputs(buf, stderr);
    fflush(stderr);
}

void err_exit(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    output_err(true, errno, true, "Error", fmt, args);
    va_end(args);
    _exit(EXIT_FAILURE);
}

void errexit(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    output_err(true, errno, true, "Error", fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

void usage_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    output_err(false, errno, true, "Usage", fmt, args);
    va_end(args);
    _exit(EXIT_FAILURE);
}

void err_msg(const char *fmt, ...)
{
    int saved_errno;
    va_list args;
    saved_errno = errno;
    output_err(true, errno, true, "Msg", fmt, args);
    va_end(args);

    errno = saved_errno;
}

void fatal(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    output_err(false, errno, true, "Fatal", fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}
