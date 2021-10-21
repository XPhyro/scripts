#include "../include/sysutil.h"

int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; rmkparent(argv[i++], 0755));

    return 0;
}
