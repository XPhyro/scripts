#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    int i, n, a;
    struct winsize size;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    a = size.ws_col * size.ws_row;

    for (n = 0;;) {
        for (i = 1; i < argc; i++) {
            if (n++ >= a)
                return 0;
            if (fputs(argv[i], stdout) == EOF)
                return 1;
        }
    }

    return 0;
}
