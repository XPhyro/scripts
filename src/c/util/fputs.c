#include <stdio.h>
#include <stdlib.h>

#define DIE(ERRMSG) { fputs("fputs: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }


int main(int argc, char *argv[])
{
    int i;

    if (argc < 2)
        DIE("no argument given");

    for (i = 1; i < argc - 1; i++) {
        fputs(argv[i], stdout);
        putchar(' ');
    }
    fputs(argv[argc - 1], stdout);

    return 0;
}
