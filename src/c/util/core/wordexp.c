#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wordexp.h>

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
    int i;
    char *line = NULL;
    size_t size;
    ssize_t len;
    wordexp_t result;

    if (argc < 2) {
        while ((len = getline(&line, &size, stdin)) != -1) {
            if (line[len - 1] == '\n')
                line[len - 1] = '\0';
            safewordexp(line, &result, 0);
        }
    } else for (i = 1; i < argc; i++)
        safewordexp(argv[i], &result, 0);

    return 0;
}
