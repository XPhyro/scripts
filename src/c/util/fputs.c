#include <stdio.h>

int main(int argc, char *argv[])
{
    int i;

    if (argc < 2) {
        fputs("At least 1 argument is needed, exiting.\n", stderr);
        return 1;
    }

    for (i = 1; i < argc - 1; i++) {
        fputs(argv[i], stdout);
        fputs(" ", stdout);
    }
    fputs(argv[argc - 1], stdout);

    return 0;
}
