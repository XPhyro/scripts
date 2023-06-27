#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    int i;
    char *s;

    for (i = 1; i < argc; i++) {
        if (!(s = strrchr(argv[i], '.'))) {
            puts(argv[i]);
        } else {
            *s = '\0';
            puts(s + 1);
        }
    }

    return 0;
}
