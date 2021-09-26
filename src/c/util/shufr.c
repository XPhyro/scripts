#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <unistd.h>

#define SETLINES(stream) while ((len = getdelim(&line, &size, delim, stream)) != -1) { \
                             tmpline = malloc((len - 1) * sizeof(char)); \
                             memcpy(tmpline, line, len - 1); \
                             lines[i++] = tmpline; \
                             if (i == n) \
                                 lines = realloc(line, (n *= 2) * sizeof(char *)); \
                         }

int main(int argc, char *argv[])
{
    int n = 100, i, tmpi, nfile, j, nsame;
    const char delim = '\0';
    char *line = NULL, *tmpline = NULL;
    char **lines = malloc(n * sizeof(char *)), **files;
    size_t size;
    ssize_t len;
    FILE *file;

    while ((i = getopt(argc, argv, "")) != -1);

    files = argv + optind;
    nfile = argc - optind;

    i = 0;
    if (nfile == 0) {
        SETLINES(stdin);
    } else for (j = 0; j < nfile; j++) {
        file = fopen(files[j], "r");
        SETLINES(file);
        fclose(file);
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
