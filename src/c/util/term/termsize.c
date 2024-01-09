#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(void)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    printf("%d %d\n", w.ws_xpixel, w.ws_ypixel);
    return EXIT_SUCCESS;
}
