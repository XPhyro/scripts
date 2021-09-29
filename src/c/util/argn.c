#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DIE(ERRMSG) { fputs("argn: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int parsenum(char *s)
{
    int n;
    char *endptr;

    errno = 0;
    n = strtol(s, &endptr, 10);

    if (errno) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (s == endptr)
        DIE("invalid number given");

    return n;
}

int main(int argc, char *argv[])
{
    int beg, end, inc, i;

    if (argc < 4)
        DIE("at least 3 arguments are required");

    beg = parsenum(argv[1]);
    end = parsenum(argv[2]);
    inc = parsenum(argv[3]);

    if (end < 0)
        end = argc;

    argv += 4 + beg;
    argc -= 4 + beg;

    for (i = 0; i < argc && i < end; i++)
        if (!(i % inc))
            printf("%s%c", argv[i], '\0');

    return 0;
}
