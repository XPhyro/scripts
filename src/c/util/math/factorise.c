#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdutil.h>

int main(int argc, char *argv[])
{
    int i, n;
    unsigned long long int fac, val, num;
    bool optgnu = false;

    while ((i = getopt(argc, argv, "gh")) != -1) {
        switch (i) {
            case 'g':
                optgnu = true;
                break;
            case 'h':
                puts("Usage: factorise [OPTION]... [NUMBER]...\n"
                     "Factorise numbers.\n"
                     "\n"
                     "  -g        emulate GNU factor\n"
                     "  -h        display this help and exit");
                exit(EXIT_SUCCESS);
                break;
            default:
                fputs("Try 'factorise -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    argv += optind;
    argc -= optind;

    if (!optgnu) {
        for (i = 0; i < argc; i++) {
            num = astrtoull(argv[i], "invalid number\n");
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
    } else {
        for (i = 0; i < argc; i++) {
            printf("%llu:", num = astrtoull(argv[i], "invalid number\n"));
            if (num < 2)
                continue;
            putchar(' ');

            while (!(num % 2)) {
                num /= 2;
                putchar('2');
                if (num != 1)
                    putchar(' ');
            }

            for (fac = 3; fac <= num; fac += 2) {
                while (!(num % fac)) {
                    num /= fac;
                    printf("%llu", fac);
                    if (num != 1)
                        putchar(' ');
                }
            }

            putchar('\n');
        }
    }

    return 0;
}
