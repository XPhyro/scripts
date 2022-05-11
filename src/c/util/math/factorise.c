#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdutil.h>

int main(int argc, char *argv[])
{
    int i, n;
    unsigned long long int fac, val, num;

    for (i = 1; i < argc; i++) {
        num = astrtoull(argv[i++], "invalid argument\n");
        if (num < 2) {
            puts("invalid argument");
            exit(EXIT_FAILURE);
        }

        for (fac = 2, val = num; fac <= val; fac++) {
            for (n = 0; !(val % fac); n++)
                val /= fac;
            if (n)
                printf("%llu: %llu %d\n", num, fac, n);
        }
    }

    return 0;
}
