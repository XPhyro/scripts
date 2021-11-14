#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    int i, n, a;
    char c, *s;
    struct winsize size;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    a = size.ws_col * size.ws_row;

    for (n = 0;;) {
        for (i = 1; i < argc; i++) {
            for (s = argv[i]; (c = *s); s++) {
                if (n++ >= a) {
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
