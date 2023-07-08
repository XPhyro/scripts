#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++) {
        if (access(argv[i], F_OK))
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
