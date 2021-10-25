#include <stdio.h>
#include <stdlib.h>

#include "../include/stdutil.h"

#define EXECNAME "argn"
#define DIE(ERRMSG) { fputs(EXECNAME": "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    int beg, end, inc, i;
    const char *const numerr = EXECNAME": invalid number given\n";

    if (argc < 4)
        DIE("at least 3 arguments are required");

    beg = astrtol(argv[1], numerr);
    end = astrtol(argv[2], numerr);
    inc = astrtol(argv[3], numerr);

    if (end < 0)
        end = argc;

    argv += 4 + beg;
    argc -= 4 + beg;

    for (i = 0; i < argc && i < end; i++)
        if (!(i % inc))
            printf("%s%c", argv[i], '\0');

    return 0;
}
