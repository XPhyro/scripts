#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    char *s, *endptr;
    long long n;

    errno = 0;
    if ((s = getenv("SHELL_VERBOSE")) && (n = strtoll(s, &endptr, 10)) > 0 && !errno && s != endptr)
        puts("set -x");

    return 0;
}
