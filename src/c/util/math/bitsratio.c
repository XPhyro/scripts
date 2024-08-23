#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    ssize_t nread, i;
    size_t totread, popcount;
    char buf[PIPE_BUF];
#if defined(__GNUC__) || defined(__clang__)
    unsigned int c;
#else
    char c;
#endif

    popcount = 0;
    totread = 0;
    while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0) {
        totread += nread * 8;
        for (i = 0; i < nread; i++) {
            c = buf[i];
#if defined(__GNUC__) || defined(__clang__)
            popcount += __builtin_popcount(c & 0xff);
#else
            popcount += ((c & (1 << 0)) >> 0) + ((c & (1 << 1)) >> 1) + ((c & (1 << 2)) >> 2) +
                        ((c & (1 << 3)) >> 3) + ((c & (1 << 4)) >> 4) + ((c & (1 << 5)) >> 5) +
                        ((c & (1 << 6)) >> 6) + ((c & (1 << 7)) >> 7);
#endif
        }
    }

    printf("%f\n", popcount / (double)totread);

    return 0;
}
