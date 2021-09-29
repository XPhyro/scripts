#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DIE(ERRMSG) { fputs("fputsn: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    unsigned long long n, i;
    char *endptr;

    if (argc != 3)
        DIE("exactly 2 arguments are accepted");

    errno = 0;
    n = strtoull(argv[2], &endptr, 10);

    if (errno) {
        perror("strtoull");
        return 2;
    }
    if (argv[2] == endptr)
        DIE("invalid number given");

    for (i = 0; i < n; i++)
        fputs(argv[1], stdout);

    return 0;
}
