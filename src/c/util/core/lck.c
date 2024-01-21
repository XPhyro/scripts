#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <xph/database.h>
#include <xph/exec_info.h>
#include <xph/stdlib.h>
#include <xph/string.h>

DEFINE_EXEC_INFO()

int main(int argc, char *argv[])
{
    int i, ret;
    lckdb_t lcktype = LCKDB_TEMP;
    bool optstatus = false, optsleep = false, optlck = true, optprint = false, optretry = false,
         optquiet = false;
    char optdelim = '\n';
    bool (*func)(const char *, lckdb_t);
    const char *s;
    char *path;
    struct timespec optretrytime = { .tv_sec = 0, .tv_nsec = 500000000 };
    struct stat st;

    if (!argc)
        return EXIT_SUCCESS;

    init_exec_info(argc, argv);

    while ((i = getopt(argc, argv, "dhilprS:s:t:uqz0")) != -1) {
        switch (i) {
            case 'd':
                optstatus = true;
                break;
            case 'h':
                printf(
                    "Usage: %s [OPTION...] [HASH...]\n"
                    "\n"
                    "If HASH contains '/', the part after the last '/' is used.\n"
                    "\n"
                    "  -d        indicate lock status via exit codes instead of managing locks. if all hashes are locked, exit code is 0, otherwise it is 1. no guarantee is made that the information provided will be correct at exit time, locks are checked sequentially and only once.\n"
                    "  -h        display this help and exit\n"
                    "  -i        use nanosleep(3p) instead of inotify(7)\n"
                    "  -l        lock lock (default)\n"
                    "  -p        print to-be locks and do nothing else\n"
                    "  -r        retry until lock is acquired. ignored if -u is given.\n"
                    "  -S NSEC   retry interval in nanoseconds, cumulative with -s. ignored if -ir is not given.\n"
                    "  -s SEC    retry interval in seconds, cumulative with -S. ignored if -ir is not given.\n"
                    "  -t TYPE   type of lock. can be one of {t, tmp, temp, temporary} or {p, prs, pers, persistent}. default is temporary.\n"
                    "  -u        unlock lock\n"
                    "  -q        do not use stdout\n"
                    "  -z        line delimiter is nul instead of newline\n"
                    "  -0        line delimiter is nul instead of newline\n",
                    execname);
                break;
            case 'i':
                optsleep = true;
                break;
            case 'l':
                optlck = true;
                break;
            case 'p':
                optprint = true;
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
                optlck = false;
                break;
            case 'q':
                optquiet = true;
                break;
            case 'z':
            case '0':
                optdelim = '\0';
                break;
            default:
                fprintf(stderr, "Try '%s -h' for more information.\n", execname);
                return EXIT_FAILURE;
        }
    }

    argc -= optind;
    argv += optind;

    if (optprint) {
        for (i = 0; i < argc; i++) {
            path = lckpath(argv[i], lcktype, false);
            printf("%s%c", path, optdelim);
            free(path);
        }
        return EXIT_SUCCESS;
    }

    if (optstatus) {
        for (i = 0; i < argc; i++) {
            path = lckpath(argv[i], lcktype, false);
            if (stat(path, &st) || !S_ISDIR(st.st_mode))
                return EXIT_FAILURE;
            free(path);
        }
        return EXIT_SUCCESS;
    }

    if (optlck && optretry && !optsleep) {
        for (i = 0; i < argc; alckdb(argv[i++], lcktype)) {}
        ret = EXIT_SUCCESS;
    } else {
        func = optlck ? lckdb : ulckdb;
        for (ret = EXIT_SUCCESS, i = 0; i < argc;) {
            if (!func((s = strrchr(argv[i], '/')) ? s + 1 : argv[i], lcktype)) {
                if (optretry && optlck) {
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
        printf("%d%c", i, optdelim);

    return ret;
}
