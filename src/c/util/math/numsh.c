/* @LDFLAGS -lm
 */

#define _POSIX_C_SOURCE 200809L

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <hedley.h>
#include <mathutil.h>
#include <stdutil.h>
#include <strutil.h>

#define EXECNAME "numsh"

#define LEN(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

#define DECLMAP(FUNC) bool map_##FUNC(int, double *, double *, double *)
#define DECLREDUCE(FUNC) bool reduce_##FUNC(int, double *, double *, double *)

#define FUNCMAP(FUNC)                                           \
    {                                                           \
        .name = #FUNC, .func = map_##FUNC, .type = FUNCTYPE_MAP \
    }
#define FUNCREDUCE(FUNC)                                              \
    {                                                                 \
        .name = #FUNC, .func = reduce_##FUNC, .type = FUNCTYPE_REDUCE \
    }

typedef enum {
    FUNCTYPE_MAP = 0,
    FUNCTYPE_REDUCE = 1,
} FUNCTYPE;

typedef struct function {
    char *name;
    bool (*func)(int, double *, double *, double *);
    FUNCTYPE type;
} __attribute__((packed)) function;

DECLMAP(acos);
DECLMAP(asin);
DECLMAP(atan);
DECLMAP(cbrt);
DECLMAP(ceil);
DECLMAP(cos);
DECLMAP(cosh);
DECLMAP(exp);
DECLMAP(fabs);
DECLMAP(floor);
DECLMAP(log);
DECLMAP(log10);
DECLMAP(log2);
DECLMAP(pow);
DECLMAP(round);
DECLMAP(sin);
DECLMAP(sinh);
DECLMAP(sqrt);
DECLMAP(tan);
DECLMAP(tanh);
DECLMAP(trunc);
DECLMAP(nonzero);

DECLREDUCE(max);
DECLREDUCE(min);
DECLREDUCE(sum);

function functions[] = {
    FUNCMAP(acos),    FUNCMAP(asin),   FUNCMAP(atan),
    FUNCMAP(cbrt),    FUNCMAP(ceil),   FUNCMAP(cos),
    FUNCMAP(cosh),    FUNCMAP(exp),    { .name = "abs", .func = map_fabs, .type = FUNCTYPE_MAP },
    FUNCMAP(floor),   FUNCMAP(log),    FUNCMAP(log10),
    FUNCMAP(log2),    FUNCMAP(pow),    FUNCMAP(round),
    FUNCMAP(sin),     FUNCMAP(sinh),   FUNCMAP(sqrt),
    FUNCMAP(tan),     FUNCMAP(tanh),   FUNCMAP(trunc),
    FUNCMAP(nonzero),

    FUNCREDUCE(max),  FUNCREDUCE(min), FUNCREDUCE(sum),
};

HEDLEY_NO_RETURN void die(const char *fmt, ...)
{
    va_list ap;

    fputs(EXECNAME ": ", stderr);

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    putchar('\n');

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int i, j, n, funcargc;
    char *line = NULL;
    size_t size, funckwargc = 0;
    ssize_t len;
    function *func = NULL;
    double *funcargv = NULL, *funckwargv = NULL, result;

    while ((i = getopt(argc, argv, "f:hLo:")) != -1) {
        switch (i) {
            case 'f': /* TODO: support multiple functions */
                for (j = 0; j < LEN(functions); j++) {
                    if (streq(functions[j].name, optarg)) {
                        func = functions + j;
                        goto funcavail;
                    }
                }
                die("invalid function given");
funcavail:
                break;
            case 'h':
                fputs(
                    "Usage: " EXECNAME " [OPTION]... [NUMBER]...\n"
                    "Do mathematical calculations on all given numbers.\n"
                    "\n"
                    "With no NUMBER, read standard input. Empty lines are ignored\n"
                    "when reading standard input.\n"
                    "\n"
                    "Option -f must be given. If FUNC requires additional arguments,\n"
                    "option -o must be given exactly as many times as required.\n" /* TODO: enforce this */
                    "\n"
                    "  -f  FUNC  function to pass numbers through. pass -L to see the list.\n"
                    "  -h        display this help and exit\n"
                    "  -L        list all supported functions and exit\n"
                    "  -o  ARG   pass additional arguments to FUNC. this option can be\n"
                    "            given multiple times, and has to be given after -f.\n",
                    stdout);
                exit(EXIT_SUCCESS);
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
            case 'o':
                funckwargv = arealloc(funckwargv, ++funckwargc * sizeof(double));
                funckwargv[funckwargc - 1] = astrtod(optarg, EXECNAME ": invalid number given\n");
                break;
            default:
                fputs("Try '" EXECNAME " -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
        }
    }

    if (!func)
        die("no function given");

    argv += optind;
    argc -= optind;

    if (!argc) {
        for (i = 0, n = 1; (len = getline(&line, &size, stdin)) != -1;) {
            if (!len)
                continue;
            if (line[len - 1] == '\n')
                line[len - 1] = '\0';
            if (++i == n)
                funcargv = arealloc(funcargv, (n *= 2) * sizeof(double));
            funcargv[i - 1] = astrtod(line, EXECNAME ": invalid number given\n");
        }
        funcargv = arealloc(funcargv, (funcargc = i) * sizeof(double));
    } else {
        funcargv = amalloc(sizeof(double) * argc);
        for (i = 0; i < argc; i++)
            funcargv[i] = astrtod(argv[i], EXECNAME ": invalid number given\n");
        funcargc = argc;
    }

    switch (func->type) {
        case FUNCTYPE_MAP:
            for (i = 0; i < funcargc; i++) {
                if (func->func(1, funcargv + i, funckwargv, &result))
                    printf("%.16g\n", result);
            }
            break;
        case FUNCTYPE_REDUCE:
            if (func->func(funcargc, funcargv, funckwargv, &result))
                printf("%.16g\n", result);
            break;
        default:
            fprintf(stderr, "unknown function type: %d\n", func->type);
            exit(EXIT_FAILURE);
    }

    free(funcargv);
    free(funckwargv);

    return 0;
}

bool map_acos(int argc, double *argv, double *kwargv, double *result)
{
    *result = acos(argv[0]);
    return true;
}

bool map_asin(int argc, double *argv, double *kwargv, double *result)
{
    *result = asin(argv[0]);
    return true;
}

bool map_atan(int argc, double *argv, double *kwargv, double *result)
{
    *result = atan(argv[0]);
    return true;
}

bool map_cbrt(int argc, double *argv, double *kwargv, double *result)
{
    *result = cbrt(argv[0]);
    return true;
}

bool map_ceil(int argc, double *argv, double *kwargv, double *result)
{
    *result = ceil(argv[0]);
    return true;
}

bool map_cos(int argc, double *argv, double *kwargv, double *result)
{
    *result = cos(argv[0]);
    return true;
}

bool map_cosh(int argc, double *argv, double *kwargv, double *result)
{
    *result = cosh(argv[0]);
    return true;
}

bool map_exp(int argc, double *argv, double *kwargv, double *result)
{
    *result = exp(argv[0]);
    return true;
}

bool map_fabs(int argc, double *argv, double *kwargv, double *result)
{
    *result = fabs(argv[0]);
    return true;
}

bool map_floor(int argc, double *argv, double *kwargv, double *result)
{
    *result = floor(argv[0]);
    return true;
}

bool map_log(int argc, double *argv, double *kwargv, double *result)
{
    *result = log(argv[0]);
    return true;
}

bool map_log10(int argc, double *argv, double *kwargv, double *result)
{
    *result = log10(argv[0]);
    return true;
}

bool map_log2(int argc, double *argv, double *kwargv, double *result)
{
    *result = log2(argv[0]);
    return true;
}

bool map_pow(int argc, double *argv, double *kwargv, double *result)
{
    *result = pow(argv[0], kwargv[0]);
    return true;
}

bool map_round(int argc, double *argv, double *kwargv, double *result)
{
    *result = round(argv[0]);
    return true;
}

bool map_sin(int argc, double *argv, double *kwargv, double *result)
{
    *result = sin(argv[0]);
    return true;
}

bool map_sinh(int argc, double *argv, double *kwargv, double *result)
{
    *result = sinh(argv[0]);
    return true;
}

bool map_sqrt(int argc, double *argv, double *kwargv, double *result)
{
    *result = sqrt(argv[0]);
    return true;
}

bool map_tan(int argc, double *argv, double *kwargv, double *result)
{
    *result = tan(argv[0]);
    return true;
}

bool map_tanh(int argc, double *argv, double *kwargv, double *result)
{
    *result = tanh(argv[0]);
    return true;
}

bool map_trunc(int argc, double *argv, double *kwargv, double *result)
{
    *result = trunc(argv[0]);
    return true;
}

bool map_nonzero(int argc, double *argv, double *kwargv, double *result)
{
    if (approxzero(argv[0]))
        return false;

    *result = argv[0];
    return true;
}

bool reduce_max(int argc, double *argv, double *kwargv, double *result)
{
    int i;
    double max =
#ifdef INFINITY
        (double)-INFINITY
#else
        -HUGE_VAL
#endif
        ;

    for (i = 0; i < argc; i++) {
        if (argv[i] > max)
            max = argv[i];
    }

    *result = max;
    return true;
}

bool reduce_min(int argc, double *argv, double *kwargv, double *result)
{
    int i;
    double min =
#ifdef INFINITY
        (double)INFINITY
#else
        HUGE_VAL
#endif
        ;

    for (i = 0; i < argc; i++) {
        if (argv[i] < min)
            min = argv[i];
    }

    *result = min;
    return true;
}

bool reduce_sum(int argc, double *argv, double *kwargv, double *result)
{
    int i;
    double sum = 0;

    for (i = 0; i < argc; sum += argv[i++]) {}

    *result = sum;
    return true;
}
