#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    unsigned long long n, i;
    char *endptr;

    if (argc != 3) {
        fputs("Exactly 2 arguments are accepted, exiting.\n", stderr);
        return 1;
    }

    n = strtoull(argv[2], &endptr, 10);

    if (argv[2] == endptr) {
        fputs("Invalid number given, exiting.\n", stderr);
        return 2;
    }

    for (i = 0; i < n; i++) {
        printf("%s", argv[1]);
    }

    return 0;
}
