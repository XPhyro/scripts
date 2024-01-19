#ifndef HEADER_SCRIPTS_C_DIE_
#define HEADER_SCRIPTS_C_DIE_

#include <stdarg.h>
#include <stdio.h>

#include <exec_info.h>

void die(const char *restrict format, ...)
{
    va_list ap;
    fprintf(stderr, "%s: ", execname);
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fputc('\n', stderr);
}

#endif /* ifndef HEADER_SCRIPTS_C_DIE_ */
