#include <string.h>
#include <sys/param.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    unsigned char data[PIPE_BUF];
    memset(data, UCHAR_MAX, PIPE_BUF * sizeof(unsigned char));
    for (;; write(STDOUT_FILENO, data, PIPE_BUF)) {}
}
