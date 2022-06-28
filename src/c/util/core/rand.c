#define _POSIX_C_SOURCE 200809L

#include <alloca.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <unistd.h>

#include <stdutil.h>

#define EXECNAME "rand"
#define RSIZE 64
_Static_assert(RSIZE == 64, "Other values are not supported in the rest of the code.");

int main(int argc, char *argv[])
{
    int i, n = 1;
    void *r = alloca(RSIZE);
    bool optsigned = false;

    while ((i = getopt(argc, argv, "hn:s")) != -1) {
        switch (i) {
            case 'h':
                puts(
                    "Usage: " EXECNAME " [OPTION]...\n"
                    "Generate cryptographically secure 64-bit random numbers on Linux 3.17 or later.\n"
                    "\n"
                    "On Linux 3.19 or later, " EXECNAME
                    " might not immediately react to SIGINT depending on CPU load.\n"
                    "\n"
                    "  -h         display this help and exit\n"
                    "  -n COUNT   count of random numbers to generate\n"
                    "  -s         generate a signed number\n"
                    "\n"
                    "Exit Codes\n"
                    "   0         Succeeded\n"
                    "   1         Interrupted during random number generation\n"
                    "   2         An error occurred");
                exit(EXIT_SUCCESS);
                break;
            case 'n':
                n = astrtol(optarg, EXECNAME ": invalid number given\n");
                break;
            case 's':
                optsigned = true;
                break;
            default:
                fputs("Try '" EXECNAME " -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

#ifndef __clang_analyzer__
    argv += optind;
    argc -= optind;
#endif /* ifndef __clang_analyzer__ */

#define PRINTRANDOM(SPECIFIER, TYPE)                \
    {                                               \
        for (i = 0; i < n; i++) {                   \
            errno = 0;                              \
            getrandom(r, RSIZE, 0);                 \
            if (errno) {                            \
                exit(errno == EINTR ? 1 : 2);       \
            }                                       \
            printf("%" SPECIFIER "\n", *(TYPE *)r); \
        }                                           \
    }

    if (optsigned)
        PRINTRANDOM(PRId64, int64_t)
    else
        PRINTRANDOM(PRIu64, uint64_t)

    return 0;
}
