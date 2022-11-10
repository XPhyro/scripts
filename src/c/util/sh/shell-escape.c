#include <stdio.h>

int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++) {
        char *s;
        char c;
        for (s = argv[i]; (c = *s); s++) {
            if (c != '\'')
                printf("'%c'", c);
            else
                fputs("\"'\"", stdout);
        }
        if (i != argc - 1)
            putchar(' ');
    }

    return 0;
}
