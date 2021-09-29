#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DIE(ERRMSG) { fputs("putsn: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    unsigned long long n, i;
    char *endptr;

    if (argc == 2) {
        argv--;
        argv[1] = "";
    } else if (argc != 3)
        DIE("exactly 2 or 3 arguments are accepted");

    errno = 0;
    n = strtoull(argv[2], &endptr, 10);

    if (errno) {
        perror("strtoull");
        exit(EXIT_FAILURE);
    }
    if (argv[2] == endptr)
        DIE("invalid number given");

    for (i = 0; i < n; i++)
        puts(argv[1]);

    return 0;
}
