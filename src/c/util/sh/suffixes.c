#include <stdio.h>

#include <xph/string.h>

int main(int argc, const char *argv[])
{
    int i;

    for (i = 2; i < argc; i++)
        if (strsfx(argv[1], argv[i]))
            return 0;

    return 1;
}
