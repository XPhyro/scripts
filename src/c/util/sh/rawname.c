#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    int i;
    char *s;

    for (i = 1; i < argc; i++) {
        if ((s = strrchr(argv[i], '.')))
            *s = '\0';
        puts(argv[i]);
    }

    return 0;
}
