#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdutil.h>
#include <strutil.h>

#define ARRLEN(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

typedef unsigned long long ull;

typedef struct {
    ull div;
    const char *name;
} divn_t;

int main(int argc, char *argv[])
{
    int i, j;
    ull num;
    bool divisible, optprepend = false;
    char *buf;
    size_t bufsize;
    const divn_t divs[8] = {
        {.div = 3,   .name = "Fizz"    },
        { .div = 5,  .name = "Buzz"    },
        { .div = 7,  .name = "Wizz"    },
        { .div = 11, .name = "Triss"   },
        { .div = 13, .name = "Yennefer"},
        { .div = 17, .name = "Mario"   },
        { .div = 19, .name = "Claire"  },
        { .div = 23, .name = "Peach"   },
    };
    const char *execname = argv[0];

    while ((i = getopt(argc, argv, "hp")) != -1) {
        switch (i) {
            case 'h':
                printf("Usage: %s [OPTION]... [NUMBER]...\n"
                       "Map given numbers to Fizz, Buzz, Wizz...\n"
                       "\n"
                       "  -h        display this help and exit\n"
                       "  -p        prepend each line with the number being mapped\n",
                       execname);
                exit(EXIT_SUCCESS);
                break;
            case 'p':
                optprepend = true;
                break;
            default:
                fprintf(stderr, "Try '%s -h' for more information.\n", execname);
                exit(EXIT_FAILURE);
                break;
        }
    }

    argc -= optind;
    argv += optind;

    buf = NULL;
    bufsize = 0;
    for (i = 0; i < argc; i++) {
        buf =
            vstrcatbuf(buf, bufsize, 3, "given string \"", argv[i], "\" is not a valid number.\n");
        num = astrtoull(argv[i], buf);

        if (optprepend)
            printf("%llu: ", num);

        divisible = false;

        for (j = 0; j < ARRLEN(divs); j++) {
            if (num % divs[j].div == 0) {
                fputs(divs[j].name, stdout);
                divisible = true;
            }
        }

        if (!divisible)
            printf("%llu", num);

        putchar('\n');
    }
    free(buf);
    buf = NULL;

    return 0;
}
