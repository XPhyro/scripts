#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <xph/stdlib.h>

#define DIE(ERRMSG)                           \
    {                                         \
        fputs("fmapc: " ERRMSG "\n", stderr); \
        exit(EXIT_FAILURE);                   \
    }

typedef struct {
    unsigned char key;
    unsigned char val;
} binmapping_t;

int main(int argc, char *argv[])
{
    const char sep = '=';
    enum { binsize = 16, bufsize = 4096 };
    const binmapping_t binmappings[binsize] = {
        { .key = '0', .val = 0 },  { .key = '1', .val = 1 },  { .key = '2', .val = 2 },
        { .key = '3', .val = 3 },  { .key = '4', .val = 4 },  { .key = '5', .val = 5 },
        { .key = '6', .val = 6 },  { .key = '7', .val = 7 },  { .key = '8', .val = 8 },
        { .key = '9', .val = 9 },  { .key = 'a', .val = 10 }, { .key = 'b', .val = 11 },
        { .key = 'c', .val = 12 }, { .key = 'd', .val = 13 }, { .key = 'e', .val = 14 },
        { .key = 'f', .val = 15 },
    };
    unsigned char buf[bufsize];
    char *s;
    unsigned char c, p, bin;
    int i, binoffset;
    size_t len;
    ssize_t nread, n;
    bool optbin = false;

    while ((i = getopt(argc, argv, "bh")) != -1) {
        switch (i) {
            case 'b':
                optbin = true;
                break;
            case 'h':
                puts(
                    "Usage: fmapc [OPTION]... [MAPPING]...\n"
                    "Map standard input according to the given mappings. A MAPPING is in the form [CHAR]=[STR].\n"
                    "\n"
                    "  -b        map hexadecimal characters to 4-bit binary and ignore mappings. non-hexadecimal are removed.\n"
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

    if (!optbin) {
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
    } else {
        bin = 0;
        binoffset = 0;
        while ((nread = read(0, buf, bufsize)) > 0) {
            for (n = 0; n < nread; n++) {
                c = buf[n];
                for (i = 0; i < binsize; i++) {
                    p = binmappings[i].key;
                    if (c == p) {
                        bin |= binmappings[i].val << binoffset;
                        if (binoffset == 4) {
                            putchar(bin);
                            binoffset = 0;
                            bin = 0;
                        } else {
                            binoffset = 4;
                        }
                        break;
                    }
                }
            }
        }
    }

    return 0;
}
