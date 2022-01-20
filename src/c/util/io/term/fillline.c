#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    int i, n, w;
    struct winsize size;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    w = size.ws_col;

    for (n = 0;;) {
        for (i = 1; i < argc; i++) {
            if (n++ >= w)
                return 0;
            if (fputs(argv[i], stdout) == EOF)
                return 1;
        }
    }

    return 0;
}
