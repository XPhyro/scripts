#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

#include <xph/math.h>

int main(int argc, char *argv[])
{
    ssize_t nread, i;
    size_t totread, popcount;
    char buf[PIPE_BUF];

    popcount = 0;
    totread = 0;
    while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0) {
        totread += nread * 8;
        for (i = 0; i < nread; popcount += popcountu(buf[i++] & 0xff)) {}
    }

    printf("%f\n", popcount / (double)totread);

    return 0;
}
