#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <hedley.h>

#include <xph/die.h>
#include <xph/exec_info.h>
#include <xph/io.h>
#include <xph/math.h>
#include <xph/stdlib.h>

DEFINE_EXEC_INFO()

size_t parseline(char *line);
void printresult(size_t sumns, bool unitless);

int main(int argc, char *argv[])
{
    char *line;
    int i;
    char optdelim = '\n';
    size_t sumns;
    bool optunitless = false;

    init_exec_info(argc, argv);

    while ((i = getopt(argc, argv, "huz0")) != -1) {
        switch (i) {
            case 'h':
                /* TODO: help message */
                return EXIT_SUCCESS;
            case 'u':
                optunitless = true;
                break;
            case 'z':
            case '0':
                optdelim = '\0';
                break;
            default:
                return EXIT_FAILURE;
        }
    }

    argc -= optind;
    argv += optind;

    sumns = 0;
    while ((line = getstr(argc, argv, optdelim)))
        sumns += parseline(line);
    free(line);

    printresult(sumns, optunitless);

    return EXIT_SUCCESS;
}

size_t parseline(char *line)
{
    size_t ns = 0, sz;
    char *s, *pdot = NULL, *endptr, *end;
    char c;
    int i;

    i = 0;
    for (s = line; (c = *s); s++) {
        if (c == '.') {
            if (pdot)
                goto err;
            pdot = s;
        } else if (c < '0' || c > '9') {
            if (c != ':')
                goto err;
            if (pdot || ++i > 3)
                goto err;
        }
    }
    end = s;

    if (pdot && pdot[1]) {
        sz = astrtoull(pdot + 1, "invalid duration string given");
        if (sz > 999999999ull)
            goto err;
        if (pdot == line)
            return sz;
        ns = sz * ipow64(10, 10 - (end - pdot));
    }

    for (s = line;; s++) {
        c = *s;
        if (!c || c == '.')
            break;
        if (c == ':') {
            i--;
            continue;
        }
        sz = strtoull(s, &endptr, 10);
        switch (i) {
            case 3:
                sz *= 24ull;
                HEDLEY_FALL_THROUGH;
            case 2:
                sz *= 60ull;
                HEDLEY_FALL_THROUGH;
            case 1:
                sz *= 60ull;
                HEDLEY_FALL_THROUGH;
            case 0:
                sz *= 1000000000ull;
                break;
            default:
                HEDLEY_UNREACHABLE();
        }
        ns += sz;
    }

    return ns;

err:
    die("invalid duration string given: %s", line);
}

void printresult(size_t sumns, bool optunitless)
{
    size_t d, h, m, s, ns;

    if (optunitless) {
        printf("%zu\n", sumns);
        return;
    }

    d = sumns / 86400000000000ull;
    sumns %= 86400000000000ull;
    h = sumns / 3600000000000ull;
    sumns %= 3600000000000ull;
    m = sumns / 60000000000ull;
    sumns %= 60000000000ull;
    s = sumns / 1000000000ull;
    sumns %= 1000000000ull;
    ns = sumns;

    if (d)
        printf("%zu:", d);
    if (d || h)
        printf("%zu:", h);
    if (d || h || m)
        printf("%zu:", m);
    printf("%zu.%zu\n", s, ns);
}
