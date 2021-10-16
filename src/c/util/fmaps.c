#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/strutil.h"

#define DIE(ERRMSG) { fputs("fmaps: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    char delim = '\n';
    char *sep = NULL, *end = NULL, *def = NULL, *line = NULL, *eol, *s;
    char **maps;
    int i, offset, nmap;
    size_t linelen = 0;
    ssize_t nread = 0;

    while ((i = getopt(argc, argv, "d:e:hs:z0")) != -1) {
        switch (i) {
            case 'd':
                def = optarg;
                break;
            case 'e':
                end = optarg;
                break;
            case 'h':
                puts("Usage: fmaps [OPTION]... [MAPPING]...\n"
                     "Map standard input according to the given mappings. A MAPPING is in the form [KEY][SEP][VAL].\n"
                     "\n"
                     "If -d, -e or -s are not given, read the respective environment variables DEF, END and SEP.\n"
                     "\n"
                     "  -d DEF    set the default output to DEF\n"
                     "  -e END    set the end-of-line string to END. default is \\n\n"
                     "  -h        display this help and exit\n"
                     "  -s SEP    set the mapping separator to SEP. default is =\n"
                     "  -z        line delimiter is NUL, not newline\n"
                     "  -0        line delimiter is NUL, not newline");
                exit(EXIT_SUCCESS);
                break;
            case 's':
                sep = optarg;
                break;
            case 'z':
            case '0':
                delim = '\0';
                break;
            default:
                fputs("Try 'fmaps -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    maps = argv + optind;
    nmap = argc - optind;

    if (!def)
        def = getenv("DEF");
    if (!end && !(end = getenv("END")))
        end = "\n";
    if (!sep && !(sep = getenv("SEP")))
        sep = "=";
    offset = strlen(sep);

    for (i = 0; i < nmap; i++) {
        s = strstr(maps[i], sep);
        if (!s)
            DIE("MAPPING does not contain SEP");
        *s = '\0';
    }

    while ((nread = getdelim(&line, &linelen, delim, stdin)) != -1) {
        eol = line;
        while(*++eol);
        eol = eol - 1;
        if (*eol == '\n')
            *eol = '\0';
        for (i = 0; i < nmap; i++) {
            if (streq(line, maps[i])) {
                printf("%s%s", maps[i] + strlen(maps[i]) + offset, end);
                goto newline;
            }
        }
        if (!def)
            printf("%s%s", line, end);
        else
            printf("%s%s", def, end);
newline: ;
    }

    return 0;
}
