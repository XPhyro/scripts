#ifndef HEADER_SCRIPTS_C_STDLIB_
#define HEADER_SCRIPTS_C_STDLIB_

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include <hedley.h>

#define ASTRTOGENPRE(TYPE, FUNC)                       \
    TYPE a##FUNC(const char *s, const char *const err) \
    {                                                  \
        TYPE n;                                        \
        int olderrno;                                  \
        char *endptr;                                  \
                                                       \
        olderrno = errno;                              \
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

ASTRTOGENBASE(long, strtol)
ASTRTOGENBASE(long long, strtoll)
ASTRTOGENBASE(unsigned long, strtoul)
ASTRTOGENBASE(unsigned long long, strtoull)
ASTRTOGENNOBASE(float, strtof)
ASTRTOGENNOBASE(double, strtod)
ASTRTOGENNOBASE(long double, strtold)

#define ASTRTOLGENPRE(TYPE, SPECIFIER)                           \
    TYPE astrto##SPECIFIER(const char *s, const char *const err) \
    {                                                            \
        long n;                                                  \
        int olderrno;                                            \
        char *endptr;                                            \
                                                                 \
        olderrno = errno;                                        \
        errno = 0;                                               \
        n = strtol(s, &endptr, 0);                               \
                                                                 \
        if (errno) {                                             \
            perror("strtol");                                    \
            exit(EXIT_FAILURE);                                  \
        }                                                        \
    if (*endptr ||
#define ASTRTOLGENPOST(TYPE) \
       )                     \
    {                        \
        fputs(err, stderr);  \
        exit(EXIT_FAILURE);  \
    }                        \
                             \
    errno = olderrno;        \
    return (TYPE)n;          \
    }

#define ASTRTOLGEN(TYPE, SPECIFIER, LIMIT_MIN, LIMIT_MAX) \
    ASTRTOLGENPRE(TYPE, SPECIFIER)                        \
    (n < LIMIT_MIN || n > LIMIT_MAX) ASTRTOLGENPOST(TYPE)

#define ASTRTOULGEN(TYPE, SPECIFIER, LIMIT) \
    ASTRTOLGENPRE(TYPE, SPECIFIER)          \
    (n > LIMIT) ASTRTOLGENPOST(TYPE)

ASTRTOLGEN(char, c, CHAR_MIN, CHAR_MAX)
ASTRTOLGEN(short, h, SHRT_MIN, SHRT_MAX)
ASTRTOULGEN(unsigned char, cu, UCHAR_MAX)
ASTRTOULGEN(unsigned short, hu, USHRT_MAX)

HEDLEY_MALLOC void *amalloc(size_t size)
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

void *areallocfit(void *ptr, size_t oldsize, size_t size)
{
    return size <= oldsize ? ptr : arealloc(ptr, size);
}

HEDLEY_MALLOC void *acalloc(size_t nmemb, size_t size)
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

HEDLEY_MALLOC void *acrealloc(void *oldptr, size_t oldnmemb, size_t nmemb, size_t size)
{
    void *ptr;

    ptr = acalloc(nmemb, size);
    memcpy(ptr, oldptr, MIN(oldnmemb, nmemb) * size);
    free(oldptr);

    return ptr;
}

#ifdef _ISOC11_SOURCE
HEDLEY_MALLOC void *aaligned_alloc(size_t alignment, size_t size)
{
    int olderrno;
    void *ptr;

    olderrno = errno;
    errno = 0;
    ptr = aligned_alloc(alignment, size);

    if (!ptr || errno) {
        perror("aligned_alloc");
        exit(EXIT_FAILURE);
    }

    errno = olderrno;

    return ptr;
}
#endif /* ifdef _ISOC11_SOURCE */

#endif /* ifndef HEADER_SCRIPTS_C_STDLIB_ */
