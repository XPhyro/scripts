#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <xph/exec_info.h>
#include <xph/io.h>
#include <xph/math.h>
#include <xph/stdlib.h>
#include <xph/string.h>

#include <hedley.h>

DEFINE_EXEC_INFO()

typedef long long int sum_t;
#define SUM_T_FMT "%llu"

#define PREFIX_KILO 1000ull
#define PREFIX_MEGA 1000000ull
#define PREFIX_GIGA 1000000000ull
#define PREFIX_TERA 1000000000000ull
#define PREFIX_PETA 1000000000000000ull

#define PREFIX_KIBI 0x400ull
#define PREFIX_MEBI 0x100000ull
#define PREFIX_GIBI 0x40000000ull
#define PREFIX_TEBI 0x10000000000ull
#define PREFIX_PEBI 0x4000000000000ull

enum {
    pfxsize = 5,
};

const sum_t decpfx[pfxsize] = {
    PREFIX_KILO, PREFIX_MEGA, PREFIX_GIGA, PREFIX_TERA, PREFIX_PETA,
};
const sum_t binpfx[pfxsize] = {
    PREFIX_KIBI, PREFIX_MEBI, PREFIX_GIBI, PREFIX_TEBI, PREFIX_PEBI,
};
const char lowerpfx[pfxsize] = { 'k', 'm', 'g', 't', 'p' };
const char upperpfx[pfxsize] = { 'K', 'M', 'G', 'T', 'P' };

bool optdec = false, optboth = false, optargisline = false, optunitless = false;
long optfig = 2;
char optdelim = '\n';

HEDLEY_NO_RETURN void die(const char *fmt, ...)
{
    va_list ap;

    fprintf(stderr, "%s: ", execname);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    exit(EXIT_FAILURE);
}

void parseargs(int *restrict argc, char **restrict argv[])
{
    int i;

    while ((i = getopt(*argc, *argv, "bdef:huz0")) != -1) {
        switch (i) {
            case 'b':
                optboth = true;
                break;
            case 'd':
                optdec = true;
                break;
            case 'e':
                optargisline = true;
                break;
            case 'f':
                if ((optfig = astrtol(optarg, "invalid number given to -f")) < 0)
                    die("invalid number given to -f\n");
                break;
            case 'h':
                printf("usage: %s [OPTION...] [FILE...]\n"
                       "       %s -e [OPTION...] [ARG...]\n"
                       "Sum human-readable sizes.\n"
                       "\n"
                       "If no FILE is given, read standard input.\n"
                       "\n"
                       "  -b      use both standard input and files if present\n"
                       "  -d      print in B instead of iB\n"
                       "  -e      treat each ARG as an input line\n"
                       "  -f FIG  decimal figure count. default is 2.\n"
                       "          FIG is clamped if it does not make\n"
                       "          sense for the printed prefix\n"
                       "  -h      display this help and exit\n"
                       "  -u      print summed size as raw byte count\n"
                       "  -z      line delimiter is NUL, not newline\n"
                       "  -0      line delimiter is NUL, not newline\n",
                       execname,
                       execname);
                exit(0);
            case 'u':
                optunitless = true;
                break;
            case 'z':
            case '0':
                optdelim = '\0';
                break;
            default:
                fprintf(stderr, "Try '%s -h' for more information.\n", execname);
                exit(EXIT_FAILURE);
        }
    }

    *argc -= optind;
    *argv += optind;
}

sum_t parse(char *s)
{
    sum_t sum, lowsum;
    char *end = NULL, *lowend = NULL;
    unsigned char npfx[pfxsize] = { 0 };
    bool dec = true;
    char c;
    int i, lowdigits;
    const sum_t *pfx = (const sum_t *)&decpfx;

    errno = 0;
    sum = strtoll(s, &end, 10);

    if (errno) {
        perror("strtoll");
        die("could not parse line: %s\n", s);
    }

    for (;;) {
        if (!(c = *end))
            return sum;
        if (isspace(c))
            end++;
        else
            break;
    }

    if (c == '.' || c == ',') {
        if ((c = *++end) == '+' || c == '-')
            die("could not parse line: %s\n", s);

        errno = 0;
        lowsum = strtoull(end, &lowend, 10);

        if (errno && lowend == end) {
            perror("strtoull");
            die("could not parse line: %s\n", s);
        }
    } else {
        lowsum = 0;
    }

    for (; (c = *end) && !isspace(c); end++) {
        if (dec) {
            for (i = 0; i < pfxsize; i++) {
                if (c == upperpfx[i] || c == lowerpfx[i]) {
                    npfx[i]++;
                    goto next;
                }
            }
            if (c == 'I' || c == 'i') {
                dec = false;
                pfx = (const sum_t *)&binpfx;
            } else if (c == 'B' || c == 'b') {
                if (end[1])
                    die("could not parse line: %s\n", s);
                break;
            }
        } else {
            if (c != 'B' && c != 'b')
                die("could not parse line: %s\n", s);
        }
next:;
    }

    if (lowsum != 0) {
        for (; lowsum && !(lowsum % 10); lowsum /= 10) {}
        lowdigits = digitcount64(lowsum);
    } else {
        lowdigits = 0;
    }

    for (i = 0; i < pfxsize; i++) {
        while (npfx[i]) {
            sum *= pfx[i];
            lowsum *= pfx[i];
            npfx[i]--;
        }
    }

    if (lowdigits) {
        if (lowdigits > 3) {
            lowsum /= ipow64(10, lowdigits - 3);
        } else {
            lowsum *= ipow64(10, 3 - lowdigits);
        }
    }

    lowsum /= 1000;

    return sum + lowsum * signll(sum);
}

sum_t sumargv(int argc, char *argv[])
{
    int i;
    sum_t sum;

    for (sum = 0, i = 0; i < argc; sum += parse(argv[i++])) {}

    return sum;
}

sum_t sumfl(FILE *fl)
{
    char *line;
    sum_t sum;

    sum = 0;
    while ((line = fgetstr(fl, optdelim)))
        sum += parse(line);
    free(line);

    return sum;
}

int main(int argc, char *argv[])
{
    sum_t sum, maxpfx;
    const sum_t *pfx;
    long double div;
    int i, j;
    FILE *fl;
    char s2[2], s64[64];

    init_exec_info(argc, argv);

    if (!argc)
        die("0 argc not supported\n");

    parseargs(&argc, &argv);

    if (argc) {
        if (optargisline) {
            sum = sumargv(argc, argv);
        } else {
            for (sum = 0, i = 0; i < argc; i++) {
                if (!(fl = fopen(argv[i], "r")))
                    die("could not open file for reading: %s\n", argv[i]);
                sum += sumfl(fl);
            }
        }
        if (optboth)
            sum += sumfl(stdin);
    } else {
        sum = sumfl(stdin);
    }

    if (optunitless) {
        printf(SUM_T_FMT "\n", sum);
    } else {
        pfx = optdec ? decpfx : binpfx;
        maxpfx = 1;
        for (i = pfxsize - 1; i >= 0; i--) {
            if (sum >= pfx[i]) {
                maxpfx = pfx[i];
                break;
            }
        }
        div = sum / (long double)maxpfx;
        j = (i + 1) * 3;
        if (optfig > j)
            optfig = j;
        snprintf(s64, sizeof(s64) / sizeof(s64[0]), "%%.%ldLF%%s%%s\n", optfig);
        if (i < 0) {
            s2[0] = '\0';
        } else {
            s2[0] = upperpfx[i];
            s2[1] = '\0';
        }
        printf(s64, div, s2, optdec || i < 0 ? "B" : "iB");
    }

    return EXIT_SUCCESS;
}
