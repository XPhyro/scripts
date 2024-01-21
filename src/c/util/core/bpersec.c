#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <xph/exec_info.h>
#include <xph/param.h>
#include <xph/stdlib.h>

DEFINE_EXEC_INFO()

typedef enum { UNIT_BYTE, UNIT_BIT } unit_t;
typedef struct timespec timespec_t;

enum { NS_PER_SECOND = 1000000000 };

typedef struct {
    const char *time;
    const char *nread;
    const char *speed;
} names_t;

ssize_t totread = 0;
ssize_t lastread = 0;
timespec_t inittime, lasttime;
unit_t optunit = UNIT_BYTE;

void clock_gettimediff(const timespec_t *ti, const timespec_t *tf, timespec_t *td)
{
    td->tv_nsec = tf->tv_nsec - ti->tv_nsec;
    td->tv_sec = tf->tv_sec - ti->tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0) {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    } else if (td->tv_sec < 0 && td->tv_nsec > 0) {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

void printdiff(const timespec_t *ti,
               const timespec_t *tf,
               ssize_t nread,
               names_t names,
               const char *sep)
{
    static timespec_t td;

    clock_gettimediff(ti, tf, &td);

    double diff = (double)(optunit == UNIT_BYTE ? nread : nread * 8) /
                  ((double)td.tv_sec + (double)td.tv_nsec / (double)NS_PER_SECOND);

    printf("%s=%ld.%09ld %s=%zd %s=%.16e%s",
           names.time,
           td.tv_sec,
           td.tv_nsec,
           names.nread,
           nread,
           names.speed,
           diff,
           sep);
}

void printdiffs(void)
{
    static timespec_t tf;

    clock_gettime(CLOCK_REALTIME, &tf);

    printdiff(&lasttime, &tf, lastread, (names_t){ "dt", "b", "b/dt" }, " || ");
    printdiff(&inittime, &tf, totread, (names_t){ "t", "b", "b/t" }, "\n");
    lasttime = tf;
}

void onsigint(int _)
{
    printdiffs();
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    ssize_t nread;
    char buf[PIPE_BUF];
    int i;
#define DEFAULT_OPTCYCLE 10000
    unsigned int optcycle = DEFAULT_OPTCYCLE;
#define DEFAULT_WARMUP 0
    unsigned long long optwarmup = DEFAULT_WARMUP;

    init_exec_info(argc, argv);

    while ((i = getopt(argc, argv, "Bbhi:n:")) != -1) {
        switch (i) {
            case 'B':
                optunit = UNIT_BYTE;
                break;
            case 'b':
                optunit = UNIT_BIT;
                break;
            case 'h':
                printf("Usage: %s [OPTION...]\n"
                       "Measure the speed of input from stdin.\n"
                       "\n"
                       "  -B        output in bytes (default)\n"
                       "  -b        output in bits\n"
                       "  -h        display this help and exit\n"
                       "  -i NUM    ignore the first NUM bytes (default is %s)\n"
                       "  -n NUM    calculate and output speed every NUM cycles (default is %s)\n",
                       execname,
                       STRINGIFY(DEFAULT_WARMUP),
                       STRINGIFY(DEFAULT_OPTCYCLE));
                exit(EXIT_SUCCESS);
            case 'i':
                optwarmup = astrtoull(optarg, "invalid number given for option -i");
                break;
            case 'n':
                optcycle = astrtoul(optarg, "invalid number given for option -n");
                if (!optcycle) {
                    fputs("invalid number given for option -n\n", stderr);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Try '%s -h' for more information.\n", execname);
                exit(EXIT_FAILURE);
        }
    }

    if (optwarmup) {
        while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0 && (totread += nread) < optwarmup) {}
        totread = 0;
        clock_gettime(CLOCK_REALTIME, &inittime);
        lasttime = inittime;
    }

    signal(SIGINT, onsigint);

    i = 1;
    clock_gettime(CLOCK_REALTIME, &inittime);
    lasttime = inittime;
    while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0) {
        totread += nread;
        lastread += nread;
        if (!(i++ % optcycle)) {
            printdiffs();
            lastread = 0;
        }
    }
    printdiffs();

    return 0;
}
