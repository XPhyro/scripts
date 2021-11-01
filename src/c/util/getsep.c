#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 4096

void checkfl(int fd, unsigned char buf[BUFSIZE])
{
    long nread, i;

    while ((nread = read(fd, buf, BUFSIZE)))
        for (i = 0; i < nread; i++)
            if (!buf[i])
                goto hasnull;
    fputs("\\n", stdout);
    return;

hasnull:
    fputs("\\0", stdout);
}

int main(int argc, char *argv[])
{
    int i;
    unsigned char buf[BUFSIZE];

    if (argc == 1) {
        checkfl(STDIN_FILENO, buf);
    } else for (i = 1; i < argc; checkfl(open(argv[i++], O_RDONLY), buf));

    return 0;
}
