#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    int i, n, w;
    char c, *s;
    struct winsize size;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    w = size.ws_col;

    for (n = 0;;) {
        for (i = 1; i < argc; i++) {
            for (s = argv[i]; (c = *s); s++) {
                if (n++ >= w) {
                    putchar('\n');
                    return 0;
                }
                if (!putchar(c))
                    return 1;
            }
        }
    }

    return 0;
}
