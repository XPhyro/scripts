#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

#if !defined(USE_BUILTIN) && (defined(__GNUC__) || defined(__clang__))
#define USE_BUILTIN
#endif

int main(int argc, char *argv[])
{
    ssize_t nread, i;
    size_t popcount;
    char buf[PIPE_BUF];
#ifdef USE_BUILTIN
    unsigned int c;
#else
    char c;
#endif

    popcount = 0;
    while ((nread = read(STDIN_FILENO, buf, PIPE_BUF)) > 0) {
        for (i = 0; i < nread; i++) {
            c = buf[i];
#ifdef USE_BUILTIN
            popcount += __builtin_popcount(c & 0xff);
#else
            popcount += (c & 0x01) + (c & 0x02) + (c & 0x04) + (c & 0x08) + (c & 0x10) +
                        (c & 0x20) + (c & 0x40) + (c & 0x80);
#endif
        }
    }

    printf("%zu\n", popcount);

    return 0;
}
