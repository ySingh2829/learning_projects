#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdarg.h>

void err_exit(const char *fmt, ...);
void errexit(const char *fmt, ...);
void usage_error(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void fatal(const char *fmt, ...);

#endif
