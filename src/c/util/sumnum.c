#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

bool optignore = false;

long double parsenum(const char *s)
{
    long double x;
    char *endptr;

    errno = 0;
    x = strtold(s, &endptr);

    if (!optignore) {
        if (errno) {
            perror("strtod");
            exit(EXIT_FAILURE);
        }
        if (s == endptr) {
            fputs("sum: invalid number given", stderr);
        }
    }

    return x;
}

int main(int argc, char *argv[])
{
    bool optnodelim = false;
    int i;
    long double sum = 0;
    char delim = '\n';
    char *line = NULL;
    size_t size, len;

    while ((i = getopt(argc, argv, "dhiz0")) != -1) {
        switch (i) {
            case 'd':
                optnodelim = true;
                break;
            case 'h':
                puts("Usage: sum [OPTION]... [NUMBER]...\n"
                     "Sum numbers.\n"
                     "\n"
                     "With no NUMBER, read standard input.\n"
                     "\n"
                     "  -d   do not print the delimiter unless on a terminal\n"
                     "  -h   display this help and exit\n"
                     "  -i   ignore invalid numbers\n"
                     "  -z   line delimiter is NUL, not newline\n"
                     "  -0   line delimiter is NUL, not newline");
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
                fputs("Try 'sum -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    argv += optind;
    argc -= optind;

    if (!argc) {
        while ((len = getdelim(&line, &size, delim, stdin)) != -1) {
            if (len && line[len - 1] == delim)
                line[len - 1] = '\0';
            sum += parsenum(line);
        }
    } else for (i = 0; i < argc; i++) {
        sum += parsenum(argv[i]);
    }

    if (!optnodelim)
        printf("%.16Lg%c", sum, isatty(1) ? '\n' : delim);
    else if (isatty(1))
        printf("%.16Lg\n", sum);
    else
        printf("%.16Lg", sum);

    return 0;
}
