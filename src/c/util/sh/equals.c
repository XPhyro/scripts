#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <strutil.h>

int main(int argc, const char *argv[])
{
    int i;

    for (i = 2; i < argc; i++)
        if (streq(argv[1], argv[i]))
            return EXIT_SUCCESS;

    return EXIT_FAILURE;
}
