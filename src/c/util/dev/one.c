#include <limits.h>
#include <unistd.h>

#include <strutil.h>

int main(int argc, char *argv[])
{
    const void *const data = amallocset(PIPE_BUF, UCHAR_MAX);
    for (;; write(STDOUT_FILENO, data, PIPE_BUF)) {}
}
