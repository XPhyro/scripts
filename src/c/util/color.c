#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/strutil.h"

#define DIE(ERRMSG) { fputs("color: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    int i;
    long nread;
    unsigned char buf[BUFSIZE];
    char *color;

    if (argc < 2)
        DIE("no argument given");

    if (streq(argv[1], "black"))
        color = "[0;30m";
    else if (streq(argv[1], "red"))
        color = "[0;31m";
    else if (streq(argv[1], "green"))
        color = "[0;32m";
    else if (streq(argv[1], "orange"))
        color = "[0;33m";
    else if (streq(argv[1], "blue"))
        color = "[0;34m";
    else if (streq(argv[1], "purple"))
        color = "[0;35m";
    else if (streq(argv[1], "cyan"))
        color = "[0;36m";
    else if (streq(argv[1], "lgray") || streq(argv[1], "lgrey"))
        color = "[0;37m";
    else if (streq(argv[1], "dgray") || streq(argv[1], "dgrey"))
        color = "[1;30m";
    else if (streq(argv[1], "lred"))
        color = "[1;31m";
    else if (streq(argv[1], "lgreen"))
        color = "[1;32m";
    else if (streq(argv[1], "yellow"))
        color = "[1;33m";
    else if (streq(argv[1], "lblue"))
        color = "[1;34m";
    else if (streq(argv[1], "lpurple"))
        color = "[1;35m";
    else if (streq(argv[1], "lcyan"))
        color = "[1;36m";
    else if (streq(argv[1], "white"))
        color = "[1;37m";
    else
        DIE("invalid color given");

    fputs(color, stdout);
    fflush(stdout);

    if (argc == 2) {
        while ((nread = read(STDIN_FILENO, buf, BUFSIZE)))
            write(STDOUT_FILENO, buf, nread);
    } else for (i = 2; i < argc; puts(argv[i++]));

    fputs("[0m", stdout);

    return 0;
}
