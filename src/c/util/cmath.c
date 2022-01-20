#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "../include/stdutil.h"
#include "../include/strutil.h"

#define EXECNAME "cmath"
#define DIE(ERRMSG) { fputs(EXECNAME": "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

#define IFSETFUNC(FUNC) else if (streq(optarg, #FUNC)) func = FUNC

int main(int argc, char *argv[])
{
    int i;
    char *line = NULL;
    size_t size;
    ssize_t len;
    double (*func)(double x) = NULL;

    while ((i = getopt(argc, argv, "f:h")) != -1) {
        switch (i) {
            case 'f':
                if (NULL) /* for macro */
                    func = NULL;
                IFSETFUNC(ceil);
                IFSETFUNC(cos);
                IFSETFUNC(cosh);
                IFSETFUNC(exp);
                IFSETFUNC(floor);
                IFSETFUNC(log);
                IFSETFUNC(log10);
                IFSETFUNC(log2);
                IFSETFUNC(round);
                IFSETFUNC(sin);
                IFSETFUNC(sinh);
                IFSETFUNC(sqrt);
                IFSETFUNC(tan);
                IFSETFUNC(tanh);
                IFSETFUNC(trunc);
                else
                    DIE("invalid function given")
                break;
            case 'h':
                puts("Usage: "EXECNAME" [OPTION]... [NUMBER]...\n"
                     "Do mathematical calculations on all given numbers.\n"
                     "\n"
                     "With no NUMBER, read standard input. Empty lines are ignored\n"
                     "when reading standard input.\n"
                     "\n"
                     "  -f  FUNC  function to pass numbers through. most POSIX\n"
                     "            math.h functions of double to double are accepted\n"
                     "  -h        display this help and exit");
                exit(EXIT_SUCCESS);
                break;
            default:
                fputs("Try '"EXECNAME" -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (!func)
        DIE("no function given");

    argv += optind;
    argc -= optind;

    if (!argc) {
        while ((len = getline(&line, &size, stdin)) != -1) {
            if (!len)
                continue;
            if (line[len - 1] == '\n')
                line[len - 1] = '\0';
            printf("%.16g\n", func(astrtod(line, EXECNAME": invalid number given\n")));
        }
    } else for (i = 0; i < argc; i++) {
        printf("%.16g\n", func(astrtod(argv[i], EXECNAME": invalid number given\n")));
    }

    return 0;
}
