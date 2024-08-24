#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#include <xph/io.h>
#include <xph/stdlib.h>
#include <xph/sys.h>

int main(int argc, char *argv[])
{
    const int delim = '\n';
    char *line, *s, *buf;
    size_t bufsize, len;

    argc--;
    argv++;

    bufsize = 256;
    buf = amalloc(bufsize);

    while ((line = getstr(argc, argv, delim))) {
        s = simpslash(line);

        len = strlen(s);
        areallocfit(buf, bufsize, len + 1);

        dirslashbuf(s, buf);
        puts(buf);
    }
    free(line);
    if (buf)
        free(buf);

    return 0;
}
