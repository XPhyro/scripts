#include <stddef.h>
#include <sys/param.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    unsigned char data[PIPE_BUF];
    for (size_t i = 0; i < PIPE_BUF; data[i] = i & 1, i++) {}
    for (;; write(STDOUT_FILENO, data, PIPE_BUF)) {}
}
