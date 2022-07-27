#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdutil.h>

#define DIE(ERRMSG)                           \
    {                                         \
        fputs("fmapc: " ERRMSG "\n", stderr); \
        exit(EXIT_FAILURE);                   \
    }

int main(int argc, char *argv[])
{
    const char sep = '=';
    enum { bufsize = 4096 };
    unsigned char buf[bufsize];
    char *s;
    char c, p;
    int i;
    size_t len;
    ssize_t nread, n;

    while ((i = getopt(argc, argv, "h")) != -1) {
        switch (i) {
            case 'h':
                puts(
                    "Usage: fmapc [OPTION]... [MAPPING]...\n"
                    "Map standard input according to the given mappings. A MAPPING is in the form [CHAR]=[STR].\n"
                    "\n"
                    "  -h        display this help and exit");
                exit(EXIT_SUCCESS);
                break;
            default:
                fputs("Try 'fmaps -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    argv += optind;
    argc -= optind;

    for (i = 0; i < argc; i++) {
        s = argv[i];
        len = strlen(s);
        if (len < 2)
            DIE("invalid mapping: length is less than 2");
        if (s[1] != sep)
            DIE("invalid mapping: second character is not the separator");
    }

    while ((nread = read(0, buf, bufsize)) > 0) {
        for (n = 0; n < nread; n++) {
            c = buf[n];
            for (i = 0; i < argc; i++) {
                p = argv[i][0];
                if (c == p) {
                    fputs(argv[i] + 2, stdout);
                    goto nextchar;
                }
            }
            putchar(c);
nextchar:;
        }
    }

    return 0;
}
