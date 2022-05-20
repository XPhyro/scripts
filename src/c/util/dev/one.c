#include <limits.h>
#include <unistd.h>

#include <strutil.h>

int main(int argc, char *argv[])
{
    const void *const data = mallocset(PIPE_BUF, UCHAR_MAX);
    for (;; write(1, data, PIPE_BUF)) {}
}
