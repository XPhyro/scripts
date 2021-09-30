#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define DIE(ERRMSG) { fputs("shufr: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

#define SETLINES(stream) while ((len = getdelim(&line, &size, delim, stream)) != -1) { \
                             if (line[len - 1] != delim) \
                                 len++; \
                             tmpstr = malloc(len * sizeof(char)); \
                             memcpy(tmpstr, line, len); \
                             tmpstr[len - 1] = '\0'; \
                             lines[i++] = tmpstr; \
                             if (i == n) \
                                 lines = realloc(lines, (n *= 2) * sizeof(char *)); \
                         }

int main(int argc, char *argv[])
{
    int n = 100, nfile, i, j, k;
    int *ihist, *iarr;
    unsigned int lprint = 0, nprint = 0, nsame = 1, nsbuf = 2;
    char delim = '\n';
    char *line = NULL, *tmpstr = NULL;
    char **lines = malloc(n * sizeof(char *)), **files;
    size_t size;
    ssize_t len;
    FILE *file;

    while ((i = getopt(argc, argv, "hl:n:rz0")) != -1) {
        switch (i) {
            case 'h':
                puts("Usage shufr [OPTION]... [FILE]...\n"
                     "Repeat a random permutation of the input lines to standard output.\n"
                     "\n"
                     "With no FILE, read standard input.\n"
                     "\n"
                     "  -h        display this help and exit\n"
                     "  -l COUNT  output at most COUNT lines\n"
                     "  -n COUNT  make sure the last COUNT lines are different\n"
                     "  -r        ignored\n"
                     "  -z        line delimiter is NUL, not newline\n"
                     "  -0        line delimiter is NUL, not newline");
                exit(EXIT_SUCCESS);
                break;
            case 'l':
                errno = 0;
                lprint = strtoul(optarg, &tmpstr, 10);
                if (errno) {
                    perror("strtoul");
                    exit(EXIT_FAILURE);
                }
                if (optarg == tmpstr)
                    DIE("invalid number given to option -l");
                break;
            case 'n':
                errno = 0;
                nsbuf = 2 * (nsame = strtoul(optarg, &tmpstr, 10));
                if (errno) {
                    perror("strtoul");
                    exit(EXIT_FAILURE);
                }
                if (optarg == tmpstr || nsame < 1)
                    DIE("invalid number given to option -n");
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

    if (i == 0)
        DIE("no lines to repeat");

    lines = realloc(lines, (n = i) * sizeof(char *));
    ihist = malloc(nsbuf * sizeof(int));
    iarr  = malloc(nsame * sizeof(int));

    for (i = 0; i < nsbuf; i++) {
        ihist[i] = -1;
    }

    srand(time(NULL));
    for (;;) {
newcycle:
        for (i = 0; i < nsame; i++) {
            k = rand() % n;
            for (j = 0; j < nsbuf; ++j) {
                if (ihist[j] == k)
                    goto newcycle;
            }
            ihist[nsame + i] = iarr[i] = k;
        }
        for (i = 0; i < nsame; i++) {
            fputs(lines[ihist[i] = iarr[i]], stdout);
            putchar(delim);
            if (lprint && ++nprint == lprint)
                exit(EXIT_SUCCESS);
        }
        for (i = nsame; i < nsbuf; i++) {
            ihist[i] = -1;
        }
    }

    return 0;
}
