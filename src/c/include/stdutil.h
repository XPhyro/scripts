#ifndef _HEADER_STDUTIL
#define _HEADER_STDUTIL

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define ASTRTOGENPRE(TYPE, FUNC)                 \
    TYPE a##FUNC(char *s, const char *const err) \
    {                                            \
        TYPE n;                                  \
        int olderrno;                            \
        char *endptr;                            \
                                                 \
        olderrno = errno;                        \
        errno = 0;

#define ASTROGEN(TYPE, FUNC, NFUNC) \
    ASTRTOGENPRE(TYPE, FUNC)        \
    NFUNC                           \
    ASTRTOGENPOST(TYPE, FUNC)

#define ASTRTOGENNOBASE(TYPE, FUNC) ASTROGEN(TYPE, FUNC, n = FUNC(s, &endptr);)
#define ASTRTOGENBASE(TYPE, FUNC) ASTROGEN(TYPE, FUNC, n = FUNC(s, &endptr, 0);)

#define ASTRTOGENPOST(TYPE, FUNC) \
    if (errno) {                  \
        perror(#FUNC);            \
        exit(EXIT_FAILURE);       \
    }                             \
    if (*endptr) {                \
        fputs(err, stderr);       \
        exit(EXIT_FAILURE);       \
    }                             \
                                  \
    errno = olderrno;             \
    return n;                     \
    }

unsigned short astrtohu(char *s, const char *const err)
{
    unsigned long n;
    int olderrno;
    char *endptr;

    olderrno = errno;
    errno = 0;
    n = strtoul(s, &endptr, 0);

    if (errno) {
        perror("strtoul");
        exit(EXIT_FAILURE);
    }
    if (*endptr || n > 255) {
        fputs(err, stderr);
        exit(EXIT_FAILURE);
    }

    errno = olderrno;
    return (unsigned short)n;
}

ASTRTOGENBASE(long, strtol)
ASTRTOGENBASE(unsigned long, strtoul)
ASTRTOGENBASE(unsigned long long, strtoull)
ASTRTOGENNOBASE(double, strtod)
ASTRTOGENNOBASE(long double, strtold)

void *amalloc(size_t size)
{
    int olderrno;
    void *ptr;

    olderrno = errno;
    errno = 0;
    ptr = malloc(size);

    if (!ptr || errno) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    errno = olderrno;

    return ptr;
}

void *arealloc(void *ptr, size_t size)
{
    int olderrno;

    olderrno = errno;
    errno = 0;
    ptr = realloc(ptr, size);

    if (!ptr || errno) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }

    errno = olderrno;

    return ptr;
}

void *acalloc(size_t nmemb, size_t size)
{
    int olderrno;
    void *ptr;

    if (!nmemb)
        return NULL;

    olderrno = errno;
    errno = 0;
    ptr = calloc(nmemb, size);

    if (!ptr || errno) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    errno = olderrno;

    return ptr;
}

#endif /* ifndef _HEADER_STDUTIL */
