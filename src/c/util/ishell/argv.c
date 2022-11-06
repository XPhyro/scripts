#include <stdio.h>

int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++) {
        putchar('\'');

        char c;
        while ((c = *(argv[i]++))) {
            if (c != '\'')
                putchar(c);
            else
                fputs("'\"'\"'", stdout);
        }

        putchar('\'');
        if (i != argc - 1)
            putchar(' ');
    }

    return 0;
}
