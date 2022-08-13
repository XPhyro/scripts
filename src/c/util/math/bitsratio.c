#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    ssize_t nread, totread, readone, i;
    char buf[PIPE_BUF];
    char c;

    readone = 0;
    totread = 0;
    while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0) {
        totread += nread * 8;
        for (i = 0; i < nread; i++) {
            c = buf[i];
            if (c & 0x01)
                readone++;
            if (c & 0x02)
                readone++;
            if (c & 0x04)
                readone++;
            if (c & 0x08)
                readone++;
            if (c & 0x10)
                readone++;
            if (c & 0x20)
                readone++;
            if (c & 0x40)
                readone++;
            if (c & 0x80)
                readone++;
        }
    }

    printf("%f\n", readone / (double)totread);

    return 0;
}
