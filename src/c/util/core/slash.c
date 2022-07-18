#include <stdio.h>

#include <ioutil.h>
#include <sysutil.h>

int main(int argc, char *argv[])
{
    const int delim = '\n';
    char *line, *s;

    argc--;
    argv++;
    while ((line = getstr(argc, argv, delim))) {
        puts(s = dirslash(simpslash(line)));
        free(s); /* TODO: reuse */
    }
    free(line);

    return 0;
}
