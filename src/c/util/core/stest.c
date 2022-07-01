/*  This was inspired by the `stest` utility included with the suckless `dmenu`.
 *
 *  Differences from the suckless version are as so:
 *  - Different exit codes
 *  - More options
 *  - No -l option (test directory contents)
 *  - Ability to pass files as arguments
 *  - Ability to have partial inverse tests
 *
 *  The implementation shares code with the suckless stest. Options are mostly
 *  compatible with the POSIX test, and to some degree with GNU extensions.
 */

#define _POSIX_C_SOURCE 200809L

#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <magic.h>

#include <ioutil.h>
#include <strutil.h>

#ifndef S_ISVTX
#define S_ISVTX 01000
#endif /* ifndef S_ISVTX */

#define LOG(...)    \
    if (optverbose) \
    fprintf(stderr, __VA_ARGS__)

#define OPTCASE(CHAR, VAR) \
    case CHAR:             \
        VAR = true;        \
        break
#define TESTCASENOBREAK(CHAR, FUNC)      \
    case CHAR:                           \
        if (!postests[CHAR - 'A']) {     \
            postests[CHAR - 'A'] = FUNC; \
            negtests[CHAR - 'A'] = NULL; \
        } else {                         \
            postests[CHAR - 'A'] = NULL; \
            negtests[CHAR - 'A'] = FUNC; \
        }
#define TESTCASE(CHAR, FUNC)     \
    TESTCASENOBREAK(CHAR, FUNC); \
    break

#define TESTFUNCDEF(NAME) bool NAME(const char *path, struct stat *st)
#define TESTFUNC(NAME, RET) \
    TESTFUNCDEF(NAME)       \
    {                       \
        return RET;         \
    }

#define TESTCOUNTMAX ('z' - 'A')

bool optquiet = false, optinvert = false, optverbose = false, optall = false, optmagic = false;
const char *optmime, *optsmime;
struct stat newer, older;
magic_t magic;

bool (*postests[TESTCOUNTMAX])(const char *, struct stat *) = { NULL };
bool (*negtests[TESTCOUNTMAX])(const char *, struct stat *) = { NULL };

TESTFUNCDEF(testhidden)
{
    int i;
    /* if (!strcmp(path, ".") || !strcmp(path, ".."))
        return false; */
    for (i = strlen(path) - 1; i >= 0; i--) {
        if (path[i] == '/')
            continue;
        for (; i >= 0; i--) {
            if (path[i] == '/')
                return path[i + 1] == '.';
        }
        return path[0] == '.';
    }
    return false;
}
TESTFUNC(testblock, S_ISBLK(st->st_mode))
TESTFUNC(testchar, S_ISCHR(st->st_mode))
TESTFUNC(testdir, S_ISDIR(st->st_mode))
TESTFUNC(testexist, !access(path, F_OK))
TESTFUNC(testreg, S_ISREG(st->st_mode))
TESTFUNCDEF(testegid)
{
    gid_t egid;
    errno = 0;
    return !((egid = getegid()) == (gid_t)(-1) && errno) && egid == st->st_gid;
}
TESTFUNC(testgid, st->st_mode &S_ISGID)
TESTFUNC(teststicky, st->st_mode &S_ISVTX)
TESTFUNCDEF(testlink)
{
    struct stat ln;
    return !lstat(path, &ln) && S_ISLNK(ln.st_mode);
}
TESTFUNCDEF(testmstype)
{
    const char *mime;

    if (!(mime = magic_file(magic, path))) {
        if ((mime = magic_error(magic)))
            fputs(mime, stderr);
        else
            fputs("unknown libmagic error\n", stderr);

        return false;
    }

    return streq(strchr(mime, '/') + 1, optsmime);
}
TESTFUNCDEF(testmtype)
{
    const char *mime;

    if (!(mime = magic_file(magic, path))) {
        if ((mime = magic_error(magic)))
            fputs(mime, stderr);
        else
            fputs("unknown libmagic error\n", stderr);

        return false;
    }

    return strneq(mime, optmime, strchr(mime, '/') - mime);
}
TESTFUNC(testmodif, st->st_mtime > st->st_atime)
TESTFUNC(testnewer, st->st_mtime > newer.st_mtime)
TESTFUNCDEF(testeuid)
{
    uid_t euid;
    errno = 0;
    return !((euid = geteuid()) == (uid_t)(-1) && errno) && euid == st->st_uid;
}
TESTFUNC(testolder, st->st_mtime < older.st_mtime)
TESTFUNC(testfifo, S_ISFIFO(st->st_mode))
TESTFUNC(testread, !access(path, R_OK))
TESTFUNCDEF(testnonempty)
{
    DIR *dir;

    if (!postests['d' - 'A'])
        return st->st_size > 0;

    errno = 0;
    if (!(dir = opendir(path)) || !(readdir(dir) && readdir(dir) && readdir(dir)) || errno)
        return false;

    closedir(dir);

    return true;
}
TESTFUNC(testsocket, S_ISSOCK(st->st_mode))
TESTFUNC(testuid, st->st_mode &S_ISUID)
TESTFUNC(testwrite, !access(path, W_OK))
TESTFUNC(testexec, !access(path, X_OK))

bool stest(const char *path)
{
    int i;
    struct stat st;

    LOG("testing file %s\n", path);

    for (i = 0, stat(path, &st); i < TESTCOUNTMAX; i++) {
        if (postests[i]) {
            LOG("running test -%c...", i + 'A');
            if (!postests[i](path, &st)) {
                LOG(" fail\n");
                return optinvert;
            }
            LOG(" pass\n");
        }
        if (negtests[i]) {
            LOG("running inverse test -%c...", i + 'A');
            if (negtests[i](path, &st)) {
                LOG(" fail\n");
                return optinvert;
            }
            LOG(" pass\n");
        }
    }

    return !optinvert;
}

inline void printhelp(void)
{
    fputs(
        "Usage: stest [OPTION]... [PATH]...\n"
        "Filter a list of files by properties.\n"
        "\n"
        "With no PATH, read standard input.\n"
        "\n"
        "All options are compliant with the filesystem-related options of POSIX and GNU test. There are some extra options.\n"
        "\n"
        "Pass the same test option even times to invert that particular test."
        "\n"
        "  -A       indicate success only if all files passed, see 'Exit Codes'\n"
        "  -a       test whether files are hidden\n"
        "  -b       test whether files are block specials\n"
        "  -c       test whether files are character specials\n"
        "  -d       test whether files are directories\n"
        "  -e       test whether files exist\n"
        "  -f       test whether files are regular files\n"
        "  -G       test whether files are owned by the effective group ID\n"
        "  -g       test whether files have their set-group-ID flag set\n"
        "  --help   display this help and exit. this option needs to be given as the first argument\n"
        "  -h       test whether files are symbolic links\n"
        "  -k       test whether files have their sticky flag set\n"
        "  -L       test whether files are symbolic links\n"
        "  -M TYPE  test whether files' mime subtype are TYPE. requires a valid magic file installed system-wide\n"
        "  -m TYPE  test whether files' mime type are TYPE. requires a valid magic file installed system-wide\n"
        "  -N       test whether files have been modified after they were last read\n"
        "  -n FILE  test whether files are newer than FILE\n"
        "  -O       test whether files are owned by the effective user ID\n"
        "  -o FILE  test whether files are older than FILE\n"
        "  -p       test whether files are named pipes\n"
        "  -q       suppress stdout\n"
        "  -r       test whether files are readable\n"
        "  -S       test whether files are sockets\n"
        "  -s       test whether files are not empty\n"
        "  -u       test whether files have their set-user-ID flag set\n"
        "  -V       print each test and its result to stderr\n"
        "  -v       invert all tests\n"
        "  -w       test whether files are writable\n"
        "  -x       test whether files are executable\n"
        "  -z       line delimiter is NUL, not newline\n"
        "  -0       line delimiter is NUL, not newline\n"
        "\n"
        "Exit Codes\n"
        "  Default\n"
        "      0    All files passed all tests.\n"
        "      0    Some files passed all tests.\n"
        "      2    No files passed all tests.\n"
        "    127    Illegal options passed.\n"
        "  If -A Is Given\n"
        "      0    All files passed all tests.\n"
        "      1    Some files passed all tests.\n"
        "      2    No files passed all tests.\n"
        "    127    Illegal options passed.\n",
        stdout);
}

int main(int argc, char *argv[])
{
    int i;
    bool hasnomatch = false, hasmatch = false;
    char delim = '\n', *line = NULL;

    if (argc > 1 && streq(argv[1], "--help")) {
        printhelp();
        return 0;
    }

    while ((i = getopt(argc, argv, "AabcdefGghkLlM:m:Nn:Oo:pqrSsuVvwxz0")) != -1) {
        switch (i) {
            OPTCASE('A', optall);
            TESTCASE('a', testhidden);
            TESTCASE('b', testblock);
            TESTCASE('c', testchar);
            TESTCASE('d', testdir);
            TESTCASE('e', testexist);
            TESTCASE('f', testreg);
            TESTCASE('G', testegid);
            TESTCASE('g', testgid);
            TESTCASE('h', testlink);
            TESTCASE('k', teststicky);
            TESTCASE('L', testlink);
            TESTCASENOBREAK('M', testmstype);
            if (!optmagic) {
                magic = magic_open(MAGIC_MIME_TYPE);
                magic_load(magic, NULL);
                magic_compile(magic, NULL);
                optmagic = true;
            }
            optsmime = optarg;
            break;
            TESTCASENOBREAK('m', testmtype);
            if (!optmagic) {
                magic = magic_open(MAGIC_MIME_TYPE);
                magic_load(magic, NULL);
                magic_compile(magic, NULL);
                optmagic = true;
            }
            optmime = optarg;
            break;
            TESTCASE('N', testmodif);
            TESTCASENOBREAK('n', testnewer);
            stat(optarg, &newer);
            break;
            TESTCASE('O', testeuid);
            TESTCASENOBREAK('o', testolder);
            stat(optarg, &older);
            break;
            TESTCASE('p', testfifo);
            OPTCASE('q', optquiet);
            TESTCASE('r', testread);
            TESTCASE('S', testsocket);
            TESTCASE('s', testnonempty);
            TESTCASE('u', testuid);
            OPTCASE('V', optverbose);
            OPTCASE('v', optinvert);
            TESTCASE('w', testwrite);
            TESTCASE('x', testexec);
            case 'z':
            case '0':
                delim = '\0';
                break;
            default:
                fputs("Try 'stest --help' for more information.\n", stderr);
                return 127;
        }
    }

    argv += optind;
    argc -= optind;

    while ((line = getstr(argc, argv, delim))) {
        if (stest(line)) {
            if (!optquiet) {
                fputs(line, stdout);
                putchar(delim);
            }
            hasmatch = true;
        } else {
            hasnomatch = true;
        }
    }

    return !hasnomatch ? 0 : hasmatch ? (!optall ? 0 : 1) : 2;
}
