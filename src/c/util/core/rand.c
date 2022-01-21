#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <unistd.h>

#include "../../include/stdutil.h"

#define EXECNAME "rand"
#define DIE(ERRMSG) { fputs(EXECNAME": "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    int i, n = 1;
    unsigned long long int r, max = ~0;

    while ((i = getopt(argc, argv, "hn:")) != -1) {
        switch (i) {
            case 'h':
                puts("Usage: "EXECNAME" [OPTION]...\n"
                     "Generate cryptographically secure 64-bit random numbers on Linux.\n"
                     "\n"
                     "  -h         display this help and exit\n"
                     "  -n COUNT   count of random numbers to generate");
                exit(EXIT_SUCCESS);
                break;
            case 'n':
                n = astrtol(optarg, EXECNAME": invalid number given\n");
                break;
            default:
                fputs("Try '"EXECNAME" -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    argv += optind;
    argc -= optind;

    for (i = 0; i < n; i++) {
        getrandom(&r, sizeof(r), 0);
        printf("%llu\n", r % max);
    }

    return 0;
}
