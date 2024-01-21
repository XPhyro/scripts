#include <stdio.h>
#include <stdlib.h>

#include <xph/stdutil.h>

#define EXECNAME "fputsn"
#define DIE(ERRMSG)                               \
    {                                             \
        fputs(EXECNAME ": " ERRMSG "\n", stderr); \
        exit(EXIT_FAILURE);                       \
    }

int main(int argc, char *argv[])
{
    unsigned long long n, i;

    if (argc != 3)
        DIE("exactly 2 arguments are accepted");

    for (i = 0, n = astrtoull(argv[2], EXECNAME ": invalid number given\n"); i < n;
         fputs(argv[1], stdout), i++) {}

    return 0;
}
