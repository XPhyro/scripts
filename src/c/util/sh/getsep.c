#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 4096

void checkfl(const char *path, unsigned char buf[BUFSIZE])
{
    long nread, i;
    int fd;
    const char *sep = "\\n";

    fd = open(path, O_RDONLY);

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
    close(fd);
}

int main(int argc, char *argv[])
{
    int i;
    unsigned char buf[BUFSIZE];

    if (argc < 2) {
        checkfl(STDIN_FILENO, buf);
    } else
        for (i = 1; i < argc; checkfl(argv[i++], buf))
            ;

    return 0;
}
