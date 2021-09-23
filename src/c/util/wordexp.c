#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wordexp.h>

int main(int argc, char *argv[])
{
    int i;
    char *line = NULL;
    size_t size;
    wordexp_t result;

    if (argc != 1) {
        for (i = 1; i < argc; i++) {
            wordexp(argv[i], &result, 0);
            printf("%s\n", result.we_wordv[0]);
            wordfree(&result);
        }
    } else {
        while (getline(&line, &size, stdin) != -1) {
            line[strlen(line) - 1] = '\0';
            wordexp(line, &result, 0);
            printf("%s\n", result.we_wordv[0]);
            wordfree(&result);
        }
    }

    return 0;
}
