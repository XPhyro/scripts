#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *s, *endptr;

    errno = 0;
    if ((s = getenv("SHELL_VERBOSE")) && strtoll(s, &endptr, 10) > 0 && !errno && !*endptr)
        puts("set -x");

    return 0;
}
