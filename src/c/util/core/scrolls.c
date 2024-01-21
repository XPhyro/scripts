#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <sys/ioctl.h>
#ifdef __sun
#include <sys/filio.h> /* need FIONREAD */
#endif /* ifdef __sun */
#include <err.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <xph/exec_info.h>
#include <xph/paramutil.h>
#include <xph/stdutil.h>

DEFINE_EXEC_INFO()

int n, idx;
#define DEFAULTTEXTLEN 50
#define DEFAULTTEXTSEP 10
unsigned int opttextlen = DEFAULTTEXTLEN, textlen = DEFAULTTEXTLEN, opttextsep = DEFAULTTEXTSEP;
bool optkeepidx = false;
size_t linesize = 0;
ssize_t linelen;
char *line = NULL;
char *doubleline;
const char delim = '\n';

void updaten(void)
{
    if (ioctl(STDIN_FILENO, FIONREAD, &n))
        err(EXIT_FAILURE, "ioctl(FIONREAD)");
}

void updates(void)
{
    size_t oldlinesize;
    char *s;

    oldlinesize = linesize;
    linelen = getdelim(&line, &linesize, delim, stdin);
    if (delim && linelen && line[linelen - 1] == delim)
        line[--linelen] = '\0';
    textlen = MIN(opttextlen, linelen);

    if (linesize > oldlinesize)
        doubleline = arealloc(doubleline, linesize * 2 + opttextsep);

    strcpy(s = doubleline, line);
    memset(s += linelen, ' ', opttextsep);
    strcpy(s + opttextsep, line);

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

    init_exec_info(argc, argv);

    while ((i = getopt(argc, argv, "hkl:p:S:s:")) != -1) {
        switch (i) {
            case 'h':
                printf(
                    "Usage: %s [OPTION]...\n"
                    "Scroll the last line of stdin on stdout.\n"
                    "\n"
                    "When initialised, scrolls awaits the first line to start scrolling. "
                    "After that, it checks for input on stdin. "
                    "If any readable input is found, it suspends scrolling until the whole line is read.\n"
                    "\n"
                    "  -h        display this help and exit\n"
                    "  -k        try to keep current index when a new line is received\n"
                    "  -l LEN    clamp line length to LEN. default is %s\n"
                    "  -p LEN    add LEN spaces of padding after the text ending. default is %s\n"
                    "  -S NSEC   wait NSEC nanoseconds after each scroll iteration. can be combined with -s. NSEC is clamped to 999999999. default is %s\n"
                    "  -s SEC    wait SEC seconds after each scroll iteration. can be combined with -S. default is %s\n",
                    execname,
                    STRINGIFY(DEFAULTTEXTLEN),
                    STRINGIFY(DEFAULTTEXTSEP),
                    STRINGIFY(DEFAULTNSEC),
                    STRINGIFY(DEFAULTSEC));
                exit(EXIT_SUCCESS);
                break;
            case 'k':
                optkeepidx = true;
                break;
            case 'l':
                opttextlen = astrtoul(optarg, "invalid number given\n");
                break;
            case 'p':
                opttextsep = astrtoul(optarg, "invalid number given\n");
                break;
            case 'S':
                delayreq.tv_nsec = MIN(astrtoul(optarg, "invalid number given\n"), 999999999);
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

#ifndef __clang_analyzer__
    argv += optind;
    argc -= optind;
#endif /* ifndef __clang_analyzer__ */

    if (setvbuf(stdout, NULL, _IONBF, 0))
        err(EXIT_FAILURE, "setvbuf");

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
        idx = (idx + 1) % (linelen + opttextsep);

        nanosleep(&delayreq, NULL); /* ignore errors */
    }

    return 0;
}
