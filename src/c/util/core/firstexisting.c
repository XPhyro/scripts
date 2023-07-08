#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int i;
    const char *path;

    for (i = 1; i < argc; i++) {
        path = argv[i];
        if (!access(path, F_OK)) {
            puts(path);
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}
