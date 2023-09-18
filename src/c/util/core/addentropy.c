#include <fcntl.h>
#include <linux/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <unistd.h>

#include <stdutil.h>

int main(void)
{
    enum {
        bufsize = PIPE_BUF,
        readsize = bufsize * sizeof(char),
        randbufoffset = 2 * sizeof(int),
        randsize = readsize + randbufoffset,
        randbufratio = 4,
    };

    ssize_t nread;
    int randfd;
    struct rand_pool_info *randinfo;
    char *buf;

    randfd = open("/dev/random", O_WRONLY);
    if (randfd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    randinfo = amalloc(randsize);
    buf = (char *)randinfo + randbufoffset;

    while ((nread = read(STDIN_FILENO, buf, readsize)) > 0) {
        randinfo->entropy_count = nread;
        randinfo->buf_size = nread / randbufratio;
        errno = 0;
        if (ioctl(randfd, RNDADDENTROPY, randinfo) == -1 || errno) {
            perror("ioctl");
            free(randinfo);
            return EXIT_FAILURE;
        }
    }

    free(randinfo);

    return EXIT_SUCCESS;
}
