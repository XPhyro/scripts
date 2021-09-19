#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int parsenum(char *s)
{
    int n;
    char *endptr;

    errno = 0;
    n = strtol(s, &endptr, 10);

    if (errno) {
        perror("strtol");
        exit(2);
    }
    if (s == endptr) {
        fputs("Invalid number given, exiting.\n", stderr);
        exit(2);
    }

    return n;
}

int main(int argc, char *argv[])
{
    int beg, end, inc, i;

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
