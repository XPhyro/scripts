#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <time.h>
#include <unistd.h>

#include "../include/stdutil.h"

#define EXECNAME "shufr"
#define DIE(...) { fputs(EXECNAME": ", stderr); \
                   fprintf(stderr, __VA_ARGS__); \
                   fputc('\n', stderr); \
                   exit(EXIT_FAILURE); }

#define SETLINES(STREAM) while ((len = getdelim(&line, &size, delim, STREAM)) != -1) { \
                             if (len && line[len - 1] != delim) \
                                 len++; \
                             tmpstr = malloc(len * sizeof(char)); \
                             memcpy(tmpstr, line, len); \
                             tmpstr[len - 1] = '\0'; \
                             lines[i++] = tmpstr; \
                             if (i == n) \
                                 lines = realloc(lines, (n *= 2) * sizeof(char *)); \
                         }

#define PRINT(LINE) { fputs(LINE, stdout); \
                      putchar(delim); \
                      if (optlimit && ++nprint == lprint) \
                          exit(EXIT_SUCCESS); }

int main(int argc, char *argv[])
{
    bool optlimit = false;
    int n = 100, i, j, r, *ihist, *iarr;
    unsigned int lprint = 0, nprint = 0, nsame = 2, nsbuf;
    char delim = '\n', *line = NULL, *tmpstr = NULL, **lines = malloc(n * sizeof(char *));
    size_t size;
    ssize_t len;
    FILE *file;

    while ((i = getopt(argc, argv, "hl:n:rz0")) != -1) {
        switch (i) {
            case 'h':
                puts("Usage: shufr [OPTION]... [FILE]...\n"
                     "Repeat a random permutation of the input lines to standard output.\n"
                     "\n"
                     "With no FILE, read standard input.\n"
                     "\n"
                     "  -h        display this help and exit\n"
                     "  -l COUNT  output at most COUNT lines\n"
                     "  -n COUNT  make sure the last COUNT lines have different indices\n"
                     "  -r        ignored\n"
                     "  -z        line delimiter is NUL, not newline\n"
                     "  -0        line delimiter is NUL, not newline");
                exit(EXIT_SUCCESS);
                break;
            case 'l':
                lprint = astrtoul(optarg, EXECNAME": invalid number given to option -l\n");
                optlimit = true;
                break;
            case 'n':
                nsame = astrtoul(optarg, EXECNAME": invalid number given to option -n\n");
                break;
            case 'r':
                /* ignored */
                break;
            case 'z':
            case '0':
                delim = '\0';
                break;
            default:
                fputs("Try 'shufr -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (optlimit && !lprint)
        exit(EXIT_SUCCESS);

    argv += optind;
    argc -= optind;

    i = 0;
    if (argc == 0) {
        SETLINES(stdin);
    } else for (j = 0; j < argc; j++) {
        file = fopen(argv[j], "r");
        SETLINES(file);
        fclose(file);
    }
    n = i;

    if (n == 0)
        DIE("no lines to repeat");

    lines = realloc(lines, n * sizeof(char *));
    srand(time(NULL));
    for (i = 0; i < n; i++) {
        tmpstr = lines[r = rand() % n];
        lines[r] = lines[i];
        lines[i] = tmpstr;
    }

    if (n < nsame) {
        for (i = 0; i < n; i++)
            PRINT(lines[i]);
        DIE("input line count (%u) is less than nsame (%u)", n, nsame);
    }

    if (nsame < 2) {
        for (;;)
            PRINT(lines[rand() % n]);
    }

    iarr  = malloc(nsame * sizeof(int));
    if (nsame == n) {
        for (;;) {
            for (i = 0; i < n; i++)
                PRINT(lines[i]);
        }
    }

    ihist = malloc((nsbuf = nsame * 2) * sizeof(int));
    for (i = 0; i < nsbuf; ihist[i++] = -1);

    for (;;) {
newcycle:
        for (i = 0; i < nsame; i++) {
            r = rand() % n;
            for (j = 0; j < nsbuf; ++j) {
                if (ihist[j] == r)
                    goto newcycle;
            }
            ihist[nsame + i] = iarr[i] = r;
        }
        for (i = 0; i < nsame; i++)
            PRINT(lines[ihist[i] = iarr[i]]);
        for (i = nsame; i < nsbuf; ihist[i++] = -1);
        for (i = 0; i < nsame; ihist[i] = ihist[i + 1], i++);
    }

    return 0;
}
