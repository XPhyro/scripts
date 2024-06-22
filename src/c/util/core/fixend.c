#define _POSIX_C_SOURCE 200809L

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <xph/exec_info.h>
#include <xph/param.h>

DEFINE_EXEC_INFO();

int main(int argc, char *argv[])
{
    bool optaddlf = false;
    bool optnoempty = false;
    bool optaddnul = false;
    bool empty = true;
    int i;
    unsigned char buf[PIPE_BUF];

    while ((i = getopt(argc, argv, "hnrz0")) != -1) {
        switch (i) {
            case 'h':
                printf("Usage: %s [OPTION...]\n"
                       "Fix incorrect ends as desired.\n"
                       "\n"
                       "  -h        display this help and exit\n"
                       "  -n        add LF if not present. can be combined with -0.\n"
                       "  -r        don't fix if empty\n"
                       "  -z        add NUL if not present. can be combined with -n.\n"
                       "  -0        add NUL if not present. can be combined with -n.\n",
                       execname);
                exit(EXIT_SUCCESS);
            case 'n':
                optaddlf = true;
                break;
            case 'r':
                optnoempty = true;
                break;
            case '0':
                optaddnul = true;
                break;
            default:
                fprintf(stderr, "Try '%s -h' for more information.\n", execname);
                exit(EXIT_FAILURE);
        }
    }
    argc -= optind;
    argv += optind;

    while ((i = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        empty = false;
        write(STDOUT_FILENO, buf, i);
    }

    if (!optnoempty || !empty) {
        if (optaddlf)
            putchar('\n');
        if (optaddnul)
            putchar('\0');
    }

    return EXIT_SUCCESS;
}
