#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#ifdef __sun
#include <sys/filio.h> /* need FIONREAD */
#endif /* ifdef __sun */

int main(int argc, char *argv[])
{
    int n;

    if (ioctl(STDIN_FILENO, FIONREAD, &n)) {
        puts("0");
        return 125;
    }
    printf("%d\n", n);

    return n == 0;
}
