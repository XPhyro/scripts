#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <sys/ioctl.h>
#ifdef __sun
#include <sys/filio.h> /* need FIONREAD */
#endif /* ifdef __sun */
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <stdutil.h>
#include <sys/paramsutil.h>

int n, idx;
#define DEFAULTTEXTLEN 50
unsigned int opttextlen = DEFAULTTEXTLEN, textlen = DEFAULTTEXTLEN;
bool optkeepidx = false;
size_t linesize;
ssize_t linelen;
char *line = NULL;
char *doubleline = NULL;
const char delim = '\n';

void updaten()
{
    if (ioctl(0, FIONREAD, &n))
        err(EXIT_FAILURE, "ioctl(FIONREAD)");
}

void updates()
{
    size_t oldlinesize;

    oldlinesize = linesize;
    linelen = getdelim(&line, &linesize, delim, stdin);
    if (delim && linelen && line[linelen - 1] == delim)
        line[linelen-- - 1] = '\0';
    textlen = MIN(opttextlen, linelen);

    if (linesize > oldlinesize)
        doubleline = arealloc(doubleline, linesize * 2);

    strcpy(doubleline, line);
    strcpy(doubleline + linelen, line);

    if (!optkeepidx)
        idx = 0;
    else
        idx = MIN(idx, linelen - 1);
}

int main(int argc, char *argv[])
{
    int i;
    char c;
#define DEFAULTSEC 0
#define DEFAULTNSEC 500000000
    struct timespec delayreq = { .tv_sec = DEFAULTSEC, .tv_nsec = DEFAULTNSEC };

    while ((i = getopt(argc, argv, "hl:S:s:")) != -1) {
        switch (i) {
            case 'h':
                fputs(
                    "Usage: scrolls [OPTION]...\n"
                    "Scroll the last line of stdin in stdout.\n"
                    "\n"
                    "  -h        display this help and exit\n"
                    "  -k        try to keep current index when a new line is received\n"
                    "  -l LEN    clamp line length to LEN. default is " STRINGIFY(
                        DEFAULTTEXTLEN) "\n"
                                        "  -S NSEC   nanoseconds to wait, can be combined with -s. NSEC is clamped to 999999999. default is " STRINGIFY(
                                            DEFAULTNSEC) "\n"
                                                         "  -s SEC    seconds to wait, can be combined with -S. default is " STRINGIFY(
                                                             DEFAULTSEC) "\n",
                    stdout);
                exit(EXIT_SUCCESS);
                break;
            case 'k':
                optkeepidx = true;
                break;
            case 'l':
                opttextlen = astrtoul(optarg, "invalid number given\n");
                break;
            case 'S':
                delayreq.tv_nsec = MAX(astrtoul(optarg, "invalid number given\n"), 999999999);
                break;
            case 's':
                delayreq.tv_sec = astrtoul(optarg, "invalid number given\n");
                break;
            default:
                fputs("Try 'scrolls -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    argv += optind;
    argc -= optind;

    updaten(); /* initial FIONREAD compatibility check */
    updates();
    for (;;) {
        updaten();
        if (n)
            updates();

        c = doubleline[idx + textlen];
        doubleline[idx + textlen] = '\0';
        puts(doubleline + idx);
        doubleline[idx + textlen] = c;
        idx = (idx + 1) % linelen;

        nanosleep(&delayreq, NULL); /* ignore errors */
    }

    return 0;
}
