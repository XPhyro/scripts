#define _POSIX_C_SOURCE 200809L
/* need non-standard dirent fields */
#define _GNU_SOURCE

#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <strutil.h>
#include <vector.h>

typedef struct {
    char dirname[PATH_MAX];
    char d_name[NAME_MAX + 1];
} fl_t;

/* Type        Value
 * DT_UNKNOWN      0
 * DT_FIFO         1
 * DT_CHR          2
 * DT_DIR          4
 * DT_BLK          6
 * DT_REG          8
 * DT_LNK         10
 * DT_SOCK        12
 *
 * These macros/values are glibc extensions.
 */

enum { typecount = 8 };
enum {
    unkownidx = 0,
    fifoidx = 1,
    chridx = 2,
    diridx = 3,
    blkidx = 4,
    regidx = 5,
    lnkidx = 6,
    sockidx = 7
};
int typeidxs[13] = { unkownidx, fifoidx, chridx, -1,     diridx, -1,     blkidx,
                     -1,        regidx,  -1,     lnkidx, -1,     sockidx };
bool types[typecount] = { true, true, true, true, true, true, true, true };
char *execname;
vector_t dirs;
fl_t tmpfl;

fl_t strtofl(const char *dirname, const char *d_name)
{
    fl_t fl;
    strcpy(fl.dirname, dirname);
    strcpy(fl.d_name, d_name);
    return fl;
}

void handledir(size_t idx, fl_t fl)
{
    struct dirent *ent;
    DIR *p;

    p = opendir(fl.d_name);

    while ((ent = readdir(p))) {
        if (ent->d_type == DT_DIR)
            VEC_ADD(&dirs, strtofl(vstrcat(3, fl.dirname, "/", fl.d_name), ent->d_name), fl_t);
        if (!(ent->d_type > 12 || (ent->d_type % 2 && ent->d_type != 1) || !types[ent->d_type]))
            printf("%s/%s\n", fl.dirname, ent->d_name);
    }

    closedir(p);
}

int main(int argc, char *argv[])
{
    int i;

    execname = argv[0];

    while ((i = getopt(argc, argv, "ht:")) != -1) {
        switch (i) {
            case 'h':
                printf(
                    "Usage: %s [OPTION]... [KEYCODE]...\n"
                    "Recursively read directories in increasing depth.\n"
                    "\n"
                    "  -h        display this help and exit\n"
                    "  -t TYPE   toggle printing files of type TYPE. all are turned on by default. valid types are (case-insensitive): UNKOWN, FIFO, CHR, DIR, BLK, REG, LNK, SOCK.",
                    execname);
                exit(EXIT_SUCCESS);
                break;
            case 't':
                if (strcaseeq(optarg, "UNKOWN"))
                    types[unkownidx] = !types[unkownidx];
                else if (strcaseeq(optarg, "FIFO"))
                    types[fifoidx] = !types[fifoidx];
                else if (strcaseeq(optarg, "CHR"))
                    types[chridx] = !types[chridx];
                else if (strcaseeq(optarg, "DIR"))
                    types[diridx] = !types[diridx];
                else if (strcaseeq(optarg, "BLK"))
                    types[blkidx] = !types[blkidx];
                else if (strcaseeq(optarg, "REG"))
                    types[regidx] = !types[regidx];
                else if (strcaseeq(optarg, "LNK"))
                    types[lnkidx] = !types[lnkidx];
                else if (strcaseeq(optarg, "SOCK"))
                    types[sockidx] = !types[sockidx];
                break;
            default:
                fprintf(stderr, "Try '%s -h' for more information.\n", execname);
                exit(EXIT_FAILURE);
                break;
        }
    }

    dirs = vec_new(1, 256);

    VEC_ADD(&dirs, strtofl(".", "."), fl_t);

    VEC_IT(&dirs, handledir, fl_t);

    return 0;
}
