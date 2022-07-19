#include <stdio.h>

#include <ioutil.h>
#include <sysutil.h>

int main(int argc, char *argv[])
{
    const int delim = '\n';
    char *s;

    argc--;
    argv++;
    while ((s = getstr(argc, argv, delim)))
        puts(dirslash(simpslash(s)));

    return 0;
}
