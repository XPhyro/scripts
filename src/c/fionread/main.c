/* Adapted from https://unix.stackexchange.com/a/498065/401367. */
/* Licensed under CC BY-SA 4.0 */

#include <unistd.h>
#include <poll.h>
#include <sys/ioctl.h>
#ifdef __sun
#include <sys/filio.h>
#endif
#include <err.h>

int main(int argc, char *argv[])
{
    int r;
    struct pollfd pd = { 0, POLLIN };

    if(poll(&pd, 1, -1) < 0)
        err(1, "poll");
    if(ioctl(0, FIONREAD, &r))
        err(1, "ioctl(FIONREAD)");

    if(!r)
        return 1;

    return 0;
}
