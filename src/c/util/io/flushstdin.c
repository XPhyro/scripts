#include <sys/param.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    unsigned char buf[PIPE_BUF];

    while (read(STDIN_FILENO, buf, sizeof(buf))) {}

    return 0;
}
