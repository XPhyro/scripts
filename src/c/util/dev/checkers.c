#include <limits.h>
#include <sys/param.h>
#include <unistd.h>

#include <xph/string.h>

int main(int argc, char *argv[])
{
    unsigned char data[PIPE_BUF];
    memset(data, 0x55, sizeof(data));
    for (;; write(STDOUT_FILENO, data, PIPE_BUF)) {}
}
