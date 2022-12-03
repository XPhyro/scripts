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
            if (c & 0x01)
                popcount++;
            if (c & 0x02)
                popcount++;
            if (c & 0x04)
                popcount++;
            if (c & 0x08)
                popcount++;
            if (c & 0x10)
                popcount++;
            if (c & 0x20)
                popcount++;
            if (c & 0x40)
                popcount++;
            if (c & 0x80)
                popcount++;
#endif
        }
    }

    printf("%f\n", popcount / (double)totread);

    return 0;
}
