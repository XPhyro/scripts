#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 4096

void checkfl(int fd, unsigned char buf[BUFSIZE])
{
    long nread, i;
    const char *sep = "\\n";

    while ((nread = read(fd, buf, BUFSIZE))) {
        for (i = 0; i < nread; i++) {
            if (!buf[i]) {
                sep = "\\0";
                goto out;
            }
        }
    }

out:
    fputs(sep, stdout);
}

int main(int argc, char *argv[])
{
    int i, fd;
    unsigned char buf[BUFSIZE];

    if (argc < 2) {
        checkfl(STDIN_FILENO, buf);
    } else
        for (i = 1; i < argc; i++) {
            if ((fd = open(argv[i], O_RDONLY)) == -1) {
                perror("open");
                continue;
            }
            checkfl(fd, buf);
            close(fd);
        }

    return 0;
}
