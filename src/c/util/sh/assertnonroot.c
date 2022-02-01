#include <stdio.h>
#include <zconf.h>

int main(int argc, char *argv[])
{
    int i;

    if (geteuid())
        return 0;

    if (argc < 2)
        fputs("This script must not be run as root.\n", stderr);
    else for (i = 1; i < argc; i++)
        fputs(argv[i], stderr);

    return 1;
}
