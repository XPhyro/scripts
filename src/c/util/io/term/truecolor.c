#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdutil.h>

#define EXECNAME "truecolor"
#define DIE(ERRMSG)                               \
    {                                             \
        fputs(EXECNAME ": " ERRMSG "\n", stderr); \
        exit(EXIT_FAILURE);                       \
    }

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    int i;
    long nread;
    unsigned char buf[BUFSIZE];
    const char *const numerr = EXECNAME ": invalid color given\n";

    if (argc < 4)
        DIE("not enough arguments given");

    printf("[38;2;%hu;%hu;%hum", astrtohu(argv[1], numerr), astrtohu(argv[2], numerr),
           astrtohu(argv[3], numerr));
    fflush(stdout);

    if (argc == 4) {
        while ((nread = read(STDIN_FILENO, buf, BUFSIZE)))
            write(STDOUT_FILENO, buf, nread);
    } else
        for (i = 4; i < argc; puts(argv[i++])) {}

    fputs("[0m", stdout);

    return 0;
}
