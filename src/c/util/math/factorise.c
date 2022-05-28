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

        for (n = 0, val = num; !(val % 2); n++)
            val /= 2;
        if (n)
            printf("%llu: 2 %d\n", num, n);

        for (fac = 3, val = num; fac <= val; fac += 2) {
            for (n = 0; !(val % fac); n++)
                val /= fac;
            if (n)
                printf("%llu: %llu %d\n", num, fac, n);
        }
    }

    return 0;
}
