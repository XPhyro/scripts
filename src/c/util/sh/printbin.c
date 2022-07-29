#include <stdio.h>
#include <stdlib.h>

#include <ioutil.h>
#include <stdutil.h>

int main(int argc, char *argv[])
{
    char *line = NULL;
    const char delim = '\n';

    argc--;
    argv++;
    while ((line = getstr(argc, argv, delim)))
        putchar(astrtol(line, "invalid number given\n"));
    free(line);

    return 0;
}
