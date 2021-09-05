#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
    double max = -INFINITY, x;
    int i;
    char *endptr;

    if (argc == 1) {
        fputs("At least 1 argument is expected, exiting.\n", stderr);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        x = strtod(argv[i], &endptr);

        if (argv[i] == endptr) {
            fputs("Invalid number given, exiting.\n", stderr);
            return 1;
        }

        if (x > max)
            max = x;
    }

    printf("%.16g\n", max);

    return 0;
}
