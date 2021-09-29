#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    char *s, *endptr;
    int n;

    if (!(s = getenv("SHELL_VERBOSE")))
        return 0;

    errno = 0;
    n = strtol(s, &endptr, 10);

    if (errno || s == endptr || n <= 0)
        return 0;

    puts("set -x");

    return 0;
}
