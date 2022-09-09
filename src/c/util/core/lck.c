#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <time.h>
#include <unistd.h>

#include <dbutil.h>
#include <stdutil.h>
#include <strutil.h>

int main(int argc, char *argv[])
{
    const char *execname;
    int i, ret;
    lckdb_t lcktype = LCKDB_TEMP;
    bool lck = true, optsleep = false, optretry = false, optquiet = false;
    bool (*func)(const char *, lckdb_t);
    char *s;
    struct timespec optretrytime = { .tv_sec = 0, .tv_nsec = 500000000 };

    if (!argc)
        return EXIT_SUCCESS;
    execname = argv[0];

    while ((i = getopt(argc, argv, "hilrS:s:t:uq")) != -1) {
        switch (i) {
            case 'h':
                printf(
                    "Usage: %s [OPTION...] [HASH...]\n"
                    "\n"
                    "If HASH contains '/', the part after the last '/' is used.\n"
                    "\n"
                    "  -h        display this help and exit\n"
                    "  -i        use nanosleep(3p) instead of inotify(7)\n"
                    "  -l        lock lock (default)\n"
                    "  -r        retry until lock is acquired. ignored if -u is given.\n"
                    "  -S NSEC   retry interval in nanoseconds, cumulative with -s. ignored if -ir is not given.\n"
                    "  -s SEC    retry interval in seconds, cumulative with -S. ignored if -ir is not given.\n"
                    "  -t TYPE   type of lock. can be one of {t, tmp, temp, temporary} or {p, prs, pers, persistent}. default is temporary.\n"
                    "  -u        unlock lock\n"
                    "  -q        do not use stdout\n",
                    execname);
                break;
            case 'i':
                optsleep = true;
                break;
            case 'l':
                lck = true;
                break;
            case 'r':
                optretry = true;
                break;
            case 'S':
                optretrytime.tv_nsec = MIN(astrtoul(optarg, "invalid number given\n"), 999999999);
                break;
            case 's':
                optretrytime.tv_sec = astrtoul(optarg, "invalid number given\n");
                break;
            case 't':
                if (streq(optarg, "t") || streq(optarg, "tmp") || streq(optarg, "temp") ||
                    streq(optarg, "temporary"))
                    lcktype = LCKDB_TEMP;
                else if (streq(optarg, "p") || streq(optarg, "prs") || streq(optarg, "pers") ||
                         streq(optarg, "persistent"))
                    lcktype = LCKDB_PERS;
                else {
                    fprintf(stderr, "%s: invalid lock type %s\n", execname, optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'u':
                lck = false;
                break;
            case 'q':
                optquiet = true;
                break;
            default:
                fprintf(stderr, "Try '%s -h' for more information.\n", execname);
                return EXIT_FAILURE;
        }
    }

    argc -= optind;
    argv += optind;

    if (lck && optretry && !optsleep) {
        for (i = 0; i < argc; alckdb(argv[i++], lcktype)) {}
        ret = EXIT_SUCCESS;
    } else {
        func = lck ? lckdb : ulckdb;
        for (ret = EXIT_SUCCESS, i = 0; i < argc;) {
            if (!func((s = strrchr(argv[i], '/')) ? s + 1 : argv[i], lcktype)) {
                if (optretry && lck) {
                    nanosleep(&optretrytime, NULL);
                    continue;
                }
                ret = EXIT_FAILURE;
                break;
            }

            i++;
        }
    }

    if (!optquiet)
        printf("%d\n", i);

    return ret;
}
