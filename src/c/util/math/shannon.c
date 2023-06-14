/* @LDFLAGS -lm
 */

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    enum { hist_size = UINT8_MAX + 1 };

    ssize_t nread, i, totread;
    uint8_t buf[PIPE_BUF];
    uintmax_t hist[hist_size] = { 0 };
    double entropy;

    totread = 0;
    while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0) {
        for (i = 0; i < nread; hist[buf[i++]]++) {}
        totread += nread;
    }

    for (entropy = 0.0, i = 0; i < hist_size; i++) {
        double p = (double)hist[i] / (double)totread;
        entropy -= p * log2(p);
    }

    printf("%f\n", entropy);

    return 0;
}
