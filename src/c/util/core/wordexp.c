#define _POSIX_C_SOURCE 200809L

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>

#include <xph/io.h>

int safewordexp(const char *restrict s, wordexp_t *restrict result, int flags)
{
    int r;

    r = wordexp(s, result, flags);
    if (!r) {
        printf("%s\n", result->we_wordv[0]);
        wordfree(result);
    }

    return r;
}

int main(int argc, char *argv[])
{
    char *line = NULL;
    wordexp_t result;

    argc--;
    argv++;

    while ((line = getstr(argc, argv, '\n')))
        safewordexp(line, &result, 0);
    free(line);

    return 0;
}
