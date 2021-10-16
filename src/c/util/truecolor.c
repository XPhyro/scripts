#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define DIE(ERRMSG) { fputs("truecolor: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

#define BUFSIZE 1024

unsigned short parsecolor(char *s)
{
    unsigned int n;
    char *endptr;

    errno = 0;
    n = strtoul(s, &endptr, 10);
    if (errno) {
        perror("strtoul");
        exit(EXIT_FAILURE);
    }
    if (s == endptr || n > 255)
        DIE("invalid color given");

    return n;
}

int main(int argc, char *argv[])
{
    int i;
    long nread;
    unsigned char buf[BUFSIZE];

    if (argc < 4)
        DIE("not enough arguments given");

    printf("[38;2;%hd;%hd;%hdm", parsecolor(argv[1]), parsecolor(argv[2]), parsecolor(argv[3]));
    fflush(stdout);

    if (argc == 4) {
        while ((nread = read(STDIN_FILENO, buf, BUFSIZE)))
            write(STDOUT_FILENO, buf, nread);
    } else for (i = 4; i < argc; puts(argv[i++]));

    fputs("[0m", stdout);

    return 0;
}
