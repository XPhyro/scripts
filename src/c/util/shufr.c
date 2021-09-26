#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int n = 100, i, tmpi;
    const char delim = '\0';
    char *line = NULL, *tmpline = NULL;
    char **lines = malloc(n * sizeof(char *));
    size_t size;
    ssize_t len;

    i = 0;
    while ((len = getdelim(&line, &size, delim, stdin)) != -1) {
        tmpline = malloc((len - 1) * sizeof(char));
        memcpy(tmpline, line, len - 1);
        lines[i++] = tmpline;
        if (i == n)
            lines = realloc(line, (n *= 2) * sizeof(char *));
    }

    lines = realloc(lines, (n = i) * sizeof(char *));

    srand(time(NULL));
    for (;;) {
        tmpi = rand() % n;
        if (tmpi == i)
            continue;
        i = tmpi;
        printf("%s%c", lines[i], delim);
    }

    return 0;
}
