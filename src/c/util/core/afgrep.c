#define _POSIX_C_SOURCE 200809L
/* need strcasestr */
#define _GNU_SOURCE

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/stdutil.h"
#include "../../include/strutil.h"

#define EXECNAME "afgrep"
#define DIE(...)                      \
    {                                 \
        fputs(EXECNAME ": ", stderr); \
        fprintf(stderr, __VA_ARGS__); \
        fputc('\n', stderr);          \
        exit(EXIT_FAILURE);           \
    }

typedef enum {
    SEARCHMODE_ANY = 0,
    SEARCHMODE_BEGIN = 1,
    SEARCHMODE_END = 2,
    SEARCHMODE_WHOLE = 4,
} SEARCHMODES;

SEARCHMODES searchmodes = SEARCHMODE_ANY;
char delim = '\n';
unsigned int offset = 0;
unsigned long long nmatch = 0, optlmatch = 0;
const char *fixedstr;
size_t fixedstrlen;
bool hasmatch = false, optinvert = false, optcase = false, optonly = false, optquiet = false;

void searchline(const char *line)
{
    const char *s, *p;
    bool match;
    size_t linelen;

    if (!fixedstrlen) {
        s = NULL; /* suppress -Wmaybe-uninitialized */
        goto match;
    }

    if (!(s = (optcase ? strcasestr : strstr)(line, fixedstr)))
        return;

    linelen = strlen(line);
    match =
        (searchmodes == SEARCHMODE_ANY) ||
        (searchmodes & SEARCHMODE_WHOLE && (linelen == fixedstrlen)) ||
        (offset <= linelen &&
         ((searchmodes & SEARCHMODE_BEGIN &&
           (s = (optcase ? strcasestr : strstr)(p = line + offset, fixedstr)) && (s == p)) ||
          (searchmodes & SEARCHMODE_END &&
           (s = (optcase ? strcaserstre : strrstre)(line, fixedstr, p = line + linelen - offset)) &&
           (s == p - 1))));

    if (optinvert ? match : !match)
        return;

match:
    hasmatch = true;
    if (optlmatch && ++nmatch > optlmatch)
        exit(EXIT_SUCCESS);

    if (!optquiet) {
        if (optonly) {
            if (fixedstrlen)
                printf("%.*s%c", (int)fixedstrlen, s, delim);
        } else {
            printf("%s%c", line, delim);
        }
    }
}

void searchfile(char **lineptr, size_t *sizeptr, FILE *fl)
{
    ssize_t len;
    char *line;
    while ((len = getdelim(lineptr, sizeptr, delim, fl)) != -1) {
        line = *lineptr;
        if (len && line[len - 1] == delim)
            line[len - 1] = '\0';
        searchline(line);
    }
}

int main(int argc, char *argv[])
{
    int i;
    char *line = NULL;
    size_t size;
    FILE *fl;

    while ((i = getopt(argc, argv, "ab:ehim:oqvxz0")) != -1) {
        switch (i) {
            case 'a':
                searchmodes |= SEARCHMODE_BEGIN;
                break;
            case 'b':
                offset = astrtoul(optarg, EXECNAME ": invalid number given to option -o\n");
                break;
            case 'e':
                searchmodes |= SEARCHMODE_END;
                break;
            case 'h':
                fputs("Usage: " EXECNAME " [OPTION]... [FIXEDSTR] [FILE]...\n"
                      "Search for FIXEDSTR with given alignment in each FILE.\n"
                      "\n"
                      "With no FILE, read standard input.\n"
                      "To include stdin in the files list, use /dev/stdin.\n"
                      "\n"
                      "If none of -aex is given, search without alignment.\n"
                      "\n"
                      "  -a         FIXEDSTR must appear at the beginning of the line\n"
                      "  -b COUNT   FIXEDSTR must appear at offset COUNT bytes from the\n"
                      "             beginning/end depending on which one of -ae is active.\n"
                      "             OFFSET must be an unsigned integer. ignored if -a or -e\n"
                      "             not given.\n"
                      "  -e         FIXEDSTR must appear at the end of the line\n"
                      "  -h         display this help and exit\n"
                      "  -i         ignore case\n"
                      "  -m COUNT   output at max COUNT lines\n"
                      "  -o         print only the matching part, not the whole line\n"
                      "  -q         suppress stdout\n"
                      "  -v         invert matches\n"
                      "  -x         FIXEDSTR must be the whole line\n"
                      "  -z         line delimiter is NUL, not newline\n"
                      "  -0         line delimiter is NUL, not newline\n",
                      stdout);
                return 0;
            case 'i':
                optcase = true;
                break;
            case 'm':
                optlmatch = astrtoul(optarg, EXECNAME ": invalid number given to option -m\n");
                break;
            case 'o':
                optonly = true;
                break;
            case 'q':
                optquiet = true;
                break;
            case 'v':
                optinvert = true;
                break;
            case 'x':
                searchmodes |= SEARCHMODE_WHOLE;
                break;
            case 'z':
            case '0':
                delim = '\0';
                break;
            default:
                fputs("Try 'afgrep -h' for more information.\n", stderr);
                return 127;
        }
    }

    argv += optind;
    argc -= optind;

    if (!argc)
        DIE("no pattern given");

    fixedstrlen = strlen(fixedstr = argv++[0]);

    if (!--argc) {
        searchfile(&line, &size, stdin);
    } else
        for (i = 0; i < argc; i++) {
            errno = 0;
            if (!(fl = fopen(argv[i], "r"))) {
                if (errno == ENOENT)
                    fprintf(stderr, EXECNAME ": %s: No such file or directory\n", argv[i]);
                else {
                    fprintf(stderr, EXECNAME ": %s: Could not open file: ", argv[i]);
                    perror("fopen");
                }
                continue;
            }
            searchfile(&line, &size, fl);
        }

    return hasmatch ? 0 : 1;
}
