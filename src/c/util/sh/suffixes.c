#include <stdio.h>

#include <strutil.h>

int main(int argc, char *argv[])
{
    int i;

    for (i = 2; i < argc; i++)
        if (strsfx(argv[1], argv[i]))
            return 0;

    return 1;
}
