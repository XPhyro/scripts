#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PRINTDIFF()                                                                              \
    {                                                                                            \
        clock_gettime(CLOCK_REALTIME, &t1);                                                      \
        clock_gettimediff(t0, t1, &td);                                                          \
        printf("%.10g\n", (double)totread /                                                      \
                              ((double)td.tv_sec + (double)td.tv_nsec / (double)NS_PER_SECOND)); \
    }

enum { NS_PER_SECOND = 1000000000 };

void clock_gettimediff(struct timespec t0, struct timespec t1, struct timespec *td)
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

int main(int argc, char *argv[])
{
    ssize_t nread;
    char buf[PIPE_BUF];
    ssize_t totread = 0;
    int i;
    struct timespec t0, t1, td;

    i = 0;
    clock_gettime(CLOCK_REALTIME, &t0);
    while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0) {
        totread += nread;
        if (!(i++ % 1000))
            PRINTDIFF()
    }
    PRINTDIFF()

    return 0;
}
