#include <stdio.h>
#include <zconf.h>

int main(int argc, char *argv[])
{
    if (geteuid()) {
        fputs("This script must be run as root.\n", stderr);
        return 1;
    }

    return 0;
}
