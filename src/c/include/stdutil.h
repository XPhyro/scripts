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
#define ASTRTOGENBASE(TYPE, FUNC) ASTROGEN(TYPE, FUNC, n = FUNC(s, &endptr, 10);)

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
    n = strtoul(s, &endptr, 10);

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
