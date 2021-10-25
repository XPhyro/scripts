#include <stdio.h>
#include <stdlib.h>

#include "../include/stdutil.h"

#define EXECNAME "putsn"
#define DIE(ERRMSG) { fputs(EXECNAME": "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    unsigned long long n, i;

    if (argc == 2) {
        argv--;
        argv[1] = "";
    } else if (argc != 3)
        DIE("exactly 2 or 3 arguments are accepted");

    for (i = 0, n = astrtoull(argv[2], EXECNAME": invalid number given\n");
         i < n;
         puts(argv[1]), i++);

    return 0;
}
