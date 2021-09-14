#include <stdio.h>

int main(int argc, char *argv[])
{
    int i;

    if (argc < 2) {
        fputs("At least 1 argument is needed, exiting.\n", stderr);
        return 1;
    }

    for (i = 1; i < argc; i++)
        puts(argv[i]);

    return 0;
}
