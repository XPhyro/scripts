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

typedef enum {
    PRINTMODE_SIGNED,
    PRINTMODE_UNSIGNED,
    PRINTMODE_BINARY,
} printmode_t;

printmode_t optprintmode = PRINTMODE_SIGNED;

void randbuf(void *buf, size_t buflen)
{
    ssize_t retlen;
#ifdef __unix__
#ifdef __linux__
    errno = 0;
    retlen = getrandom(buf, buflen, 0);
    if (errno == EINTR)
        exit(2);
    if (retlen == -1 || (errno && errno != EINTR))
        exit(1);
    if (retlen < buflen)
        exit(3);
#else /* ifdef __linux__ */
    arc4random_buf(buf, buflen);
#endif /* ifndef __linux__ */
#endif /* ifdef __unix__ */
}

int main(int argc, char *argv[])
{
    int i, n = 1;
    void *buf;
    size_t buflen = 64;

    while ((i = getopt(argc, argv, "bc:hn:su")) != -1) {
        switch (i) {
            case 'b':
                optprintmode = PRINTMODE_BINARY;
                break;
            case 'c':
                buflen = astrtoull(optarg, EXECNAME ": invalid number given\n");
                break;
            case 'h':
                puts(
                    "Usage: " EXECNAME " [OPTION]...\n"
                    "Generate cryptographically secure random bits on Linux 3.17 or later, or *BSD.\n"
                    "\n"
                    "By default, 64 bits are generated and printed as an unsigned integer.\n"
                    "\n"
                    "On Linux 3.19 or later, " EXECNAME
                    " might not immediately react to SIGINT depending on CPU load.\n"
                    "\n"
                    "On Linux, getrandom(2) and on *BSD, arc4random_buf(3) are used to generate random bits.\n"
                    "\n"
                    "On Linux, `" EXECNAME
                    " -b -c NUM` is guaranteed to be equivalent to `head -c NUM /dev/urandom` if NUM is less than or equal to 256.\n"
                    "\n"
                    "On *BSD, this program never fails.\n"
                    "\n"
                    "Options\n"
                    "  -b       print random bits as raw bytes\n"
                    "  -c NUM   generate n bits instead of 64, ignored if -b is not given\n"
                    "  -h       display this help and exit\n"
                    "  -n NUM   count of random numbers to generate\n"
                    "  -s       print random bits as a signed integer (default)\n"
                    "  -u       print random bits as an unsigned integer\n"
                    "\n"
                    "Exit Codes\n"
                    "   0       Succeeded\n"
                    "   1       An error occurred\n"
                    "   2       Interrupted during random number generation\n"
                    "   3       Could not generate enough random bits");
                exit(EXIT_SUCCESS);
                break;
            case 'n':
                n = astrtol(optarg, EXECNAME ": invalid number given\n");
                break;
            case 's':
                optprintmode = PRINTMODE_SIGNED;
                break;
            case 'u':
                optprintmode = PRINTMODE_UNSIGNED;
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

    if (optprintmode != PRINTMODE_BINARY)
        buflen = 64;
    buf = amalloc(buflen);

    for (i = 0; i < n; i++) {
        randbuf(buf, buflen);
        switch (optprintmode) {
            case PRINTMODE_SIGNED:
                printf("%" PRId64 "\n", *(int64_t *)buf);
                break;
            case PRINTMODE_UNSIGNED:
                printf("%" PRIu64 "\n", *(uint64_t *)buf);
                break;
            case PRINTMODE_BINARY:
                write(STDOUT_FILENO, buf, buflen);
                break;
        }
    }

    free(buf);

    return 0;
}
