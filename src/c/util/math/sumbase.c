#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ioutil.h>
#include <stdutil.h>

#define EXECNAME "sumbase"

bool optignore = false;
int optbase = 10;

long long int parsenum(const char *s)
{
    long long int x;
    char *endptr;

    errno = 0;
    x = strtoll(s, &endptr, optbase);

    if (errno) {
        perror("strtoll");
        goto numerr;
    }
    if (s == endptr) {
        fputs(EXECNAME ": invalid number given", stderr);
        goto numerr;
    }

    return x;

numerr:
    if (!optignore)
        exit(EXIT_FAILURE);
    return 0;
}

int main(int argc, char *argv[])
{
    bool optnodelim = false;
    int i;
    long long int sum = 0;
    char delim = '\n', *line = NULL;

    while ((i = getopt(argc, argv, "dhiz0")) != -1) {
        switch (i) {
            case 'b':
                optbase = astrtol(argv[i], EXECNAME ": invalid base given\n");
                break;
            case 'd':
                optnodelim = true;
                break;
            case 'h':
                puts("Usage: " EXECNAME " [OPTION]... [NUMBER]...\n"
                     "Sum integers in the given base.\n"
                     "\n"
                     "With no NUMBER, read standard input.\n"
                     "\n"
                     "  -b BASE   base to use (default: 10)\n"
                     "  -d        do not print the delimiter unless on a terminal\n"
                     "  -h        display this help and exit\n"
                     "  -i        ignore invalid numbers\n"
                     "  -z        line delimiter is NUL, not newline\n"
                     "  -0        line delimiter is NUL, not newline");
                exit(EXIT_SUCCESS);
                break;
            case 'i':
                optignore = true;
                break;
            case 'z':
            case '0':
                delim = '\0';
                break;
            default:
                fputs("Try '" EXECNAME " -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    argv += optind;
    argc -= optind;

    while ((line = getstr(argc, argv, delim)))
        sum += parsenum(line);
    free(line);

    if (!optnodelim)
        printf("%lld%c", sum, isatty(STDOUT_FILENO) ? '\n' : delim);
    else if (isatty(STDOUT_FILENO))
        printf("%lld\n", sum);
    else
        printf("%lld", sum);

    return 0;
}
