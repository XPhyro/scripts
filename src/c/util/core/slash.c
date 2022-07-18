#include <stdio.h>

#include <ioutil.h>
#include <sysutil.h>

int main(int argc, char *argv[])
{
    const int delim = '\n';
    char *s, *dir;

    argc--;
    argv++;
    while ((s = getstr(argc, argv, delim))) {
        dir = dirslash(simpslash(s));
        puts(dir);
        free(s);
        free(dir);
    }

    return 0;
}
