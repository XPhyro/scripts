#ifndef HEADER_SCRIPTS_C_DIE_
#define HEADER_SCRIPTS_C_DIE_

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <hedley.h>

#include "exec_info.h"

HEDLEY_NO_RETURN void die(const char *restrict format, ...)
{
    va_list ap;

    fprintf(stderr, "%s: ", execname);

    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);

    fputc('\n', stderr);

    exit(EXIT_FAILURE);
}

void die_if(bool condition, const char *restrict format, ...)
{
    va_list ap;

    if (!condition)
        return;

    va_start(ap, format);
    die(format, ap);
    va_end(ap);
}

#endif /* ifndef HEADER_SCRIPTS_C_DIE_ */
