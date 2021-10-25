#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../include/stdutil.h"

#define EXECNAME "min"
#define DIE(ERRMSG) { fputs(EXECNAME": "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    double min = INFINITY, x;
    int i;

    if (argc < 2)
        DIE("no argument given");

    for (i = 1; i < argc; i++) {
        x = astrtod(argv[i], EXECNAME": invalid number given\n");
        if (x < min)
            min = x;
    }

    printf("%.16g\n", min);

    return 0;
}
