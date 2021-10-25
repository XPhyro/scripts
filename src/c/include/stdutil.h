#include <stdlib.h>
#include <errno.h>

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
    if (s == endptr || n > 255) {
        fputs(err, stderr);
        exit(EXIT_FAILURE);
    }

    errno = olderrno;
    return (unsigned short)n;
}

long astrtol(char *s, const char *const err)
{
    long n;
    int olderrno;
    char *endptr;

    olderrno = errno;
    errno = 0;
    n = strtol(s, &endptr, 10);

    if (errno) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (s == endptr) {
        fputs(err, stderr);
        exit(EXIT_FAILURE);
    }

    errno = olderrno;
    return n;
}

unsigned long astrtoul(char *s, const char *const err)
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
    if (s == endptr) {
        fputs(err, stderr);
        exit(EXIT_FAILURE);
    }

    errno = olderrno;
    return n;
}

unsigned long long astrtoull(char *s, const char *const err)
{
    unsigned long long n;
    int olderrno;
    char *endptr;

    olderrno = errno;
    errno = 0;
    n = strtoull(s, &endptr, 10);

    if (errno) {
        perror("strtoull");
        exit(EXIT_FAILURE);
    }
    if (s == endptr) {
        fputs(err, stderr);
        exit(EXIT_FAILURE);
    }

    errno = olderrno;
    return n;
}

double astrtod(char *s, const char *const err)
{
    double n;
    int olderrno;
    char *endptr;

    olderrno = errno;
    errno = 0;
    n = strtod(s, &endptr);

    if (errno) {
        perror("strtod");
        exit(EXIT_FAILURE);
    }
    if (s == endptr) {
        fputs(err, stderr);
        exit(EXIT_FAILURE);
    }

    errno = olderrno;
    return n;
}

long double astrtold(char *s, const char *const err)
{
    long double n;
    int olderrno;
    char *endptr;

    olderrno = errno;
    errno = 0;
    n = strtold(s, &endptr);

    if (errno) {
        perror("strtold");
        exit(EXIT_FAILURE);
    }
    if (s == endptr) {
        fputs(err, stderr);
        exit(EXIT_FAILURE);
    }

    errno = olderrno;
    return n;
}
