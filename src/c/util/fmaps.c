#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

char *strnext(char *s)
{
    char *scp;
    scp = s;
    while(*(scp++));
    return scp;
}

int main(int argc, char *argv[])
{
    char *sep, *end, *line, *eol;
    int i, c;
    size_t len;
    ssize_t nread;

    end = getenv("END");
    if (!end)
        end = "\n";

    if (argc == 1) {
        while((c = getchar()) != EOF) {
            if (c != '\n')
                putchar(c);
            else
                fputs(end, stdout);
        }
        return 0;
    }

    sep = getenv("SEP");
    if (!sep)
        sep = "=";

    for (i = 1; i < argc; i++)
        argv[i] = strtok(argv[i], sep);

    line = NULL;
    len = 0;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        eol = strnext(line) - 2;
        if (*eol == '\n')
            *eol = '\0';
        for (i = 1; i < argc; i++) {
            if (!strcmp(line, argv[i])) {
                printf("%s%s", strnext(argv[i]), end);
                goto newline;
            }
        }
        printf("%s%s", line, end);
newline: ;
    }

    return 0;
}
