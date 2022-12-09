#define _POSIX_C_SOURCE 200809L

#include <ioutil.h>
#include <stdio.h>
#include <sysutil.h>

int main(int argc, char *argv[])
{
    const int delim = '\n';
    char *line, *s, *d;

    argc--;
    argv++;
    while ((line = getstr(argc, argv, delim))) {
        s = simpslash(line);
        d = dirslash(s);
        if (d) {
            puts(d);
            free(d); /* TODO: reuse */
        } else {
            puts(line);
        }
    }
    free(line);

    return 0;
}
