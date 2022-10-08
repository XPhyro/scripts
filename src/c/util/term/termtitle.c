#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
        return EXIT_FAILURE;

    printf("\033]0;%s\007", argv[1]);

    return EXIT_SUCCESS;
}
