#include <stdio.h>
#include <stdlib.h>

#define DIE(ERRMSG)                          \
    {                                        \
        fputs("puts: " ERRMSG "\n", stderr); \
        exit(EXIT_FAILURE);                  \
    }

int main(int argc, const char *argv[])
{
    int i;

    if (argc < 2)
        DIE("no argument given");

    for (i = 1; i < argc; i++)
        puts(argv[i]);

    return 0;
}
