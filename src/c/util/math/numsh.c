#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "../../include/stdutil.h"
#include "../../include/strutil.h"

#define EXECNAME "numsh"

#define DIE(ERRMSG) { fputs(EXECNAME": "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

#define LEN(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

#define DECLMAP(FUNC) double map_##FUNC(int, double *)
#define DECLREDUCE(FUNC) double reduce_##FUNC(int, double *)

#define FUNCMAP(FUNC) { #FUNC, map_##FUNC, FUNCTYPE_MAP }
#define FUNCREDUCE(FUNC) { #FUNC, reduce_##FUNC, FUNCTYPE_REDUCE }

typedef enum {
    FUNCTYPE_MAP = 0,
    FUNCTYPE_REDUCE = 1,
} FUNCTYPE;

typedef struct function {
    char *name;
    double (*func)(int, double *);
    FUNCTYPE type;
} function;

DECLMAP(acos);
DECLMAP(asin);
DECLMAP(atan);
DECLMAP(ceil);
DECLMAP(cos);
DECLMAP(cosh);
DECLMAP(exp);
DECLMAP(fabs);
DECLMAP(floor);
DECLMAP(log);
DECLMAP(log10);
DECLMAP(log2);
DECLMAP(round);
DECLMAP(sin);
DECLMAP(sinh);
DECLMAP(sqrt);
DECLMAP(tan);
DECLMAP(tanh);
DECLMAP(trunc);

DECLREDUCE(min);
DECLREDUCE(max);
DECLREDUCE(sum);

function functions[] = {
    FUNCMAP(acos),
    FUNCMAP(asin),
    FUNCMAP(atan),
    FUNCMAP(ceil),
    FUNCMAP(cos),
    FUNCMAP(cosh),
    FUNCMAP(exp),
    { "abs", map_fabs, FUNCTYPE_MAP },
    FUNCMAP(floor),
    FUNCMAP(log),
    FUNCMAP(log10),
    FUNCMAP(log2),
    FUNCMAP(round),
    FUNCMAP(sin),
    FUNCMAP(sinh),
    FUNCMAP(sqrt),
    FUNCMAP(tan),
    FUNCMAP(tanh),
    FUNCMAP(trunc),

    FUNCREDUCE(max),
    FUNCREDUCE(min),
    FUNCREDUCE(sum),
};

int main(int argc, char *argv[])
{
    int i, j;
    char *line = NULL;
    size_t size;
    ssize_t len;
    function *func = NULL;
    double *nums;

    while ((i = getopt(argc, argv, "f:hL")) != -1) {
        switch (i) {
            case 'f': /* TODO: support multiple functions */
                for (j = 0; j < LEN(functions); j++) {
                    if (streq(functions[j].name, optarg)) {
                        func = functions + j;
                        goto funcavail;
                    }
                }
                DIE("invalid function given")
funcavail:
                break;
            case 'h':
                puts("Usage: "EXECNAME" [OPTION]... [NUMBER]...\n"
                     "Do mathematical calculations on all given numbers.\n"
                     "\n"
                     "With no NUMBER, read standard input. Empty lines are ignored\n"
                     "when reading standard input.\n"
                     "\n"
                     "  -f  FUNC  function to pass numbers through. pass -L to see the list.\n"
                     "  -h        display this help and exit\n"
                     "  -L        list all supported functions and exit");
                exit(EXIT_SUCCESS);
                break;
            case 'L':
                puts("Map Functions");
                for (j = 0; j < LEN(functions); j++) {
                    if (functions[j].type != FUNCTYPE_MAP)
                        continue;
                    putchar('\t');
                    puts(functions[j].name);
                }

                puts("\nReduction Functions");
                for (j = 0; j < LEN(functions); j++) {
                    if (functions[j].type != FUNCTYPE_REDUCE)
                        continue;
                    putchar('\t');
                    puts(functions[j].name);
                }

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
        DIE("stdin is not yet supported");
        while ((len = getline(&line, &size, stdin)) != -1) {
            if (!len)
                continue;
            if (line[len - 1] == '\n')
                line[len - 1] = '\0';
            /* TODO: build `nums` */
        }
    } else {
        nums = malloc(sizeof(double) * argc);
        for (i = 0; i < argc; i++)
            nums[i] = astrtod(argv[i], EXECNAME": invalid number given\n");
    }

    switch (func->type) {
        case FUNCTYPE_MAP:
            for (i = 0; i < argc; i++) {
                printf("%.16g\n", func->func(1, nums + i));
            }
            break;
        case FUNCTYPE_REDUCE:
            printf("%.16g\n", func->func(argc, nums));
            break;
    }

    return 0;
}

double map_acos(int argc, double *argv)
{
    return acos(argv[0]);
}

double map_asin(int argc, double *argv)
{
    return asin(argv[0]);
}

double map_atan(int argc, double *argv)
{
    return atan(argv[0]);
}

double map_ceil(int argc, double *argv)
{
    return ceil(argv[0]);
}

double map_cos(int argc, double *argv)
{
    return cos(argv[0]);
}

double map_cosh(int argc, double *argv)
{
    return cosh(argv[0]);
}

double map_exp(int argc, double *argv)
{
    return exp(argv[0]);
}

double map_fabs(int argc, double *argv)
{
    return fabs(argv[0]);
}

double map_floor(int argc, double *argv)
{
    return floor(argv[0]);
}

double map_log(int argc, double *argv)
{
    return log(argv[0]);
}

double map_log10(int argc, double *argv)
{
    return log10(argv[0]);
}

double map_log2(int argc, double *argv)
{
    return log2(argv[0]);
}

double map_round(int argc, double *argv)
{
    return round(argv[0]);
}

double map_sin(int argc, double *argv)
{
    return sin(argv[0]);
}

double map_sinh(int argc, double *argv)
{
    return sinh(argv[0]);
}

double map_sqrt(int argc, double *argv)
{
    return sqrt(argv[0]);
}

double map_tan(int argc, double *argv)
{
    return tan(argv[0]);
}

double map_tanh(int argc, double *argv)
{
    return tanh(argv[0]);
}

double map_trunc(int argc, double *argv)
{
    return trunc(argv[0]);
}

double reduce_min(int argc, double *argv)
{
    int i;
    double m = INFINITY;

    for (i = 0; i < argc; i++) {
        if (argv[i] < m)
            m = argv[i];
    }

    return m;
}

double reduce_max(int argc, double *argv)
{
    int i;
    double m = -INFINITY;

    for (i = 0; i < argc; i++) {
        if (argv[i] > m)
            m = argv[i];
    }

    return m;
}

double reduce_sum(int argc, double *argv)
{
    int i;
    double s = 0;

    for (i = 0; i < argc; i++) {
        s += argv[i];
    }

    return s;
}
