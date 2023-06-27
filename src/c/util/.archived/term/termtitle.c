#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
    int i;

    if (argc < 2)
        return EXIT_FAILURE;

    fputs("\033]0;", stdout);
    for (i = 1; i < argc - 1; i++)
        printf("%s ", argv[i]);
    printf("%s\007", argv[i]);

    return EXIT_SUCCESS;
}
