#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct winsize w;
    int ret = EXIT_SUCCESS;
    FILE *fl;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    if (argc <= 1) {
        printf("%d %d\n", w.ws_xpixel, w.ws_ypixel);
    } else {
        for (int i = 1; i < argc; i++) {
            if (!(fl = fopen(argv[i], "w"))) {
                perror("fopen");
                ret = EXIT_FAILURE;
                continue;
            }

            fprintf(fl, "%d %d\n", w.ws_xpixel, w.ws_ypixel);
            fclose(fl);
        }
    }

    return ret;
}
