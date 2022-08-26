#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <paramutil.h>
#include <stdutil.h>

typedef enum { UNIT_BYTE, UNIT_BIT } unit_t;
typedef struct timespec timespec_t;

enum { NS_PER_SECOND = 1000000000 };

ssize_t totread = 0;
timespec_t t0;
unit_t optunit = UNIT_BYTE;

void clock_gettimediff(timespec_t t1, timespec_t *td)
{
    td->tv_nsec = t1.tv_nsec - t0.tv_nsec;
    td->tv_sec = t1.tv_sec - t0.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0) {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    } else if (td->tv_sec < 0 && td->tv_nsec > 0) {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

void printdiff(void)
{
    static timespec_t t1, td;

    clock_gettime(CLOCK_REALTIME, &t1);
    clock_gettimediff(t1, &td);

    printf("%.16e\n",
           (double)(optunit == UNIT_BYTE ? totread : totread * 8) /
               ((double)td.tv_sec + (double)td.tv_nsec / (double)NS_PER_SECOND));
}

void onsigint(int _)
{
    printdiff();
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    ssize_t nread;
    char buf[PIPE_BUF];
    int i;
    char *execname = argv[0];
#define DEFAULT_OPTCYCLE 10000
    unsigned int optcycle = DEFAULT_OPTCYCLE;
#define DEFAULT_WARMUP 0
    unsigned long long optwarmup = DEFAULT_WARMUP;
    bool optstart = false;

    while ((i = getopt(argc, argv, "Bbhn:")) != -1) {
        switch (i) {
            case 'B':
                optunit = UNIT_BYTE;
                break;
            case 'b':
                optunit = UNIT_BIT;
                break;
            case 'h':
                printf(
                    "Usage: %s [OPTION...]\n"
                    "Measure the speed of input from stdin.\n"
                    "\n"
                    "  -B        output in bytes (default)\n"
                    "  -b        output in bits\n"
                    "  -h        display this help and exit\n"
                    "  -i NUM    ignore the first NUM bytes (default is %s)\n",
                    "  -n NUM    calculate and output speed approximately every NUM bytes (default is %s)\n"
                    "  -s        start measuring after -i is satisfied\n",
                    execname,
                    STRINGIFY(DEFAULT_WARMUP),
                    STRINGIFY(DEFAULT_OPTCYCLE));
                exit(EXIT_SUCCESS);
            case 'n':
                optcycle = astrtoul(optarg, "invalid number given");
                break;
            case 's':
                optwarmup = astrtoull(optarg, "invalid number given");
                break;
            default:
                fprintf(stderr, "Try '%s -h' for more information.\n", execname);
                exit(EXIT_FAILURE);
        }
    }

    i = 0;
    clock_gettime(CLOCK_REALTIME, &t0);

    signal(SIGINT, onsigint);

    while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0) {
        totread += nread;
        if (optwarmup) {
            if (totread < optwarmup)
                continue;
            totread -= optwarmup;
            optwarmup = 0;
            if (optstart)
                clock_gettime(CLOCK_REALTIME, &t0);
        }
        if (optcycle && !(i++ % optcycle))
            printdiff();
    }
    printdiff();

    return 0;
}
