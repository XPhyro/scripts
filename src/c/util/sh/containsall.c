#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    int i;

    for (i = 2; i < argc; i++)
        if (!strstr(argv[1], argv[i]))
            return 1;

    return 0;
}
