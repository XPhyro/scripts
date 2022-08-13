#define _POSIX_C_SOURCE 200809L

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

void clock_gettimediff(timespec_t t0, timespec_t t1, timespec_t *td)
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

void printdiff(timespec_t t0, ssize_t totread, unit_t unit)
{
    static timespec_t t1, td;
    clock_gettime(CLOCK_REALTIME, &t1);
    clock_gettimediff(t0, t1, &td);
    printf("%.16e\n",
           (double)totread / ((double)td.tv_sec + (double)td.tv_nsec / (double)NS_PER_SECOND));
}

int main(int argc, char *argv[])
{
    ssize_t nread;
    char buf[PIPE_BUF];
    ssize_t totread = 0;
    int i;
    timespec_t t0;
    char *execname = argv[0];
    unit_t optunit = UNIT_BYTE;
#define DEFAULT_OPTCYCLE 10000
    unsigned int optcycle = DEFAULT_OPTCYCLE;

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
                    "  -n NUM    calculate and output speed approximately every NUM bytes (default is %s)\n",
                    execname, STRINGIFY(DEFAULT_OPTCYCLE));
                exit(EXIT_SUCCESS);
            case 'n':
                optcycle = astrtoul(optarg, "invalid number given");
                break;
        }
    }

    i = 0;
    clock_gettime(CLOCK_REALTIME, &t0);
    while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0) {
        totread += nread;
        if (optcycle && !(i++ % optcycle))
            printdiff(t0, totread, optunit);
    }
    printdiff(t0, totread, optunit);

    return 0;
}
