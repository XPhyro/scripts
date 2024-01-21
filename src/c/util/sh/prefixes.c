#include <stdio.h>

#include <xph/strutil.h>

int main(int argc, const char *argv[])
{
    int i;

    for (i = 2; i < argc; i++)
        if (strpfx(argv[1], argv[i]))
            return 0;

    return 1;
}
