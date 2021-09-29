#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define DIE(ERRMSG) { fputs("rand: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int parsenum(char *s)
{
    int n;
    char *endptr;

    errno = 0;
    n = strtol(s, &endptr, 10);

    if (errno) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (s == endptr || n < 1)
        DIE("invalid number given");

    return n;
}

int main(int argc, char *argv[])
{
    int i, max = RAND_MAX, n = 1;

    switch (argc) {
        case 4:
            printf("%d\n", RAND_MAX);
            exit(EXIT_SUCCESS);
            break;
        case 3:
            n = parsenum(argv[2]);
        case 2:
            max = parsenum(argv[1]);
            break;
        case 1:
            break;
        default:
            DIE("invalid number of arguments");
            break;
    }

    srand(time(NULL));

    for (i = 0; i < n; i++)
        printf("%d\n", rand() % max);

    return 0;
}
