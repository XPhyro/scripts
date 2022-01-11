#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "../include/stdutil.h"

#define EXECNAME "rand"
#define DIE(ERRMSG) { fputs(EXECNAME": "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    struct timeval tv;
    int i, max = RAND_MAX, n = 1;

    switch (argc) {
        case 4:
            printf("%d\n", RAND_MAX);
            exit(EXIT_SUCCESS);
            break;
        case 3:
            n = astrtol(argv[2], EXECNAME": invalid number given\n");
        case 2:
            max = astrtol(argv[1], EXECNAME": invalid number given\n");
            break;
        case 1:
            break;
        default:
            DIE("invalid number of arguments");
            break;
    }

    gettimeofday(&tv, NULL);
    srand(tv.tv_usec * tv.tv_sec);

    for (i = 0; i < n; i++)
        printf("%d\n", rand() % max);

    return 0;
}
