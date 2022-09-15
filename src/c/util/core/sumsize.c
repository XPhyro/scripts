#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ioutil.h>
#include <stdutil.h>
#include <strutil.h>

#include <hedley.h>

typedef unsigned long long int sum_t;
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

const char *execname;
bool optdec = false, optboth = false, optargisline = false, optunitless = false;
char *optfig = "2";
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
                if (astrtol(optarg, "invalid number given to -f") < 0)
                    die("invalid number given to -f\n");
                optfig = optarg;
                break;
            case 'h':
                printf("usage: %s [OPTION...] [FILE...]\n"
                       "       %s -e [OPTION...] [ARG...]"
                       "Sum human-readable sizes.\n"
                       "\n"
                       "If no FILE is given, read standard input.\n"
                       "\n"
                       "  -b      use both standard input and files if present\n"
                       "  -d      print in B instead of iB\n"
                       "  -e      treat each ARG as an input line\n"
                       "  -f FIG  decimal figure count. default is 2.\n"
                       "  -h      display this help and exit\n"
                       "  -u      print summed size as raw byte count\n"
                       "  -z      line delimiter is NUL, not newline\n"
                       "  -0      line delimiter is NUL, not newline\n",
                       execname,
                       execname);
                break;
            case 'u':
                optunitless = true;
                break;
            case 'z':
            case '0':
                optdelim = '\0';
                break;
            default:
                die("invalid option -- '%c'\nTry '%s -h' for more information.\n", i, execname);
        }
    }

    *argc -= optind;
    *argv += optind;
}

sum_t parse(char *s)
{
    sum_t sum;
    char *end;
    unsigned char npfx[pfxsize] = { 0 };
    bool dec = true;
    char c;
    int i;
    const sum_t *pfx = (const sum_t *)&decpfx;

    errno = 0;
    sum = strtoull(s, &end, 0);

    if (errno) {
        perror("strtoull");
        die("could not parse line: %s\n", s);
    }

    while (true) {
        if (!(c = *end))
            return sum;
        if (isspace(c))
            end++;
        else
            break;
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

    for (i = 0; i < pfxsize; i++) {
        while (npfx[i]) {
            sum *= pfx[i];
            npfx[i]--;
        }
    }

    return sum;
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
    int i;
    FILE *fl;
    char *s;

    if (!argc) {
        execname = "NULL";
        die("0 argc not supported\n");
    }
    execname = argv[0];

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
        for (i = pfxsize - 1; i >= 0; i--) {
            if (sum >= (maxpfx = pfx[i]))
                break;
        }
        div = sum / (long double)maxpfx;
        s = vstrcat(3, "%.", optfig, "Lf%c%s\n");
        printf(s, div, upperpfx[i], optdec ? "B" : "iB");
    }

    return EXIT_SUCCESS;
}
