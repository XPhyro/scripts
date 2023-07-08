#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int i;
    const char *path;

    for (i = 1; i < argc; i++) {
        path = argv[i];
        if (!access(path, F_OK)) {
            puts(path);
            return 0;
        }
    }

    return 1;
}
