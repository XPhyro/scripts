#ifndef HEADER_SCRIPTS_C_DATABASE_
#define HEADER_SCRIPTS_C_DATABASE_

#include <assert.h>
#include <errno.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <hedley.h>

#include "meta.h"
#include "string.h"
#include "sys.h"

const char *const db_scriptsprefix = "/scripts/";
const char *const db_locksprefix = "/locks/";

const char *temphome_s(const char *pfx)
{
    static const char *temphome;
    static char *lastpfx = NULL;
    char *prefix;

    if (lastpfx) {
        if (!strcmp(lastpfx, pfx))
            return temphome;

        free(lastpfx);
        lastpfx = astrdup(pfx);
    }

    if (!(prefix = getenv("TMPDIR")))
        prefix = "/tmp";

    return temphome = vstrcat(2, prefix, pfx);
}

const char *temphome(void)
{
    return temphome_s(db_scriptsprefix);
}

/* thread-unsafe */
const char *confhome_s(const char *pfx)
{
    static const char *confhome;
    static char *lastpfx = NULL;
    char *prefix, *s;
    bool allocedprefix = false;

    if (lastpfx) {
        if (!strcmp(lastpfx, pfx))
            return confhome;

        free(lastpfx);
        lastpfx = astrdup(pfx);
    }

    if (!(prefix = getenv("XDG_CONFIG_HOME"))) {
        if (!(s = getenv("HOME")) && !(s = getpwuid(getuid())->pw_dir))
            return confhome = NULL;
        prefix = vstrcat(2, s, "/.config");
        allocedprefix = true;
    }

    confhome = vstrcat(2, prefix, pfx);
    if (allocedprefix)
        free(prefix);

    rmkdir((char *)confhome, 0755);

    return confhome;
}

const char *confhome(void)
{
    return confhome_s(db_scriptsprefix);
}

/* thread-unsafe */
const char *cachehome_s(const char *pfx)
{
    static const char *cachehome;
    static char *lastpfx = NULL;
    char *prefix, *s;
    bool allocedprefix = false;

    if (lastpfx) {
        if (!strcmp(lastpfx, pfx))
            return cachehome;

        free(lastpfx);
        lastpfx = astrdup(pfx);
    }

    if (!(prefix = getenv("XDG_CACHE_HOME"))) {
        if (!(s = getenv("HOME")) && !(s = getpwuid(getuid())->pw_dir))
            return cachehome = NULL;
        prefix = vstrcat(2, s, "/.cache");
        allocedprefix = true;
    }

    cachehome = vstrcat(2, prefix, pfx);
    if (allocedprefix)
        free(prefix);

    rmkdir((char *)cachehome, 0755);

    return cachehome;
}

const char *cachehome(void)
{
    return cachehome_s(db_scriptsprefix);
}

typedef enum { LCKDB_TEMP, LCKDB_PERS } lckdb_t;

const char *lckhome_s(lckdb_t type, const char *pfx)
{
    static const char *lckhome;
    static char *lastpfx = NULL;
    char *prefix;
    const char *s;

    assert(type == LCKDB_TEMP || type == LCKDB_PERS);

    if (lastpfx) {
        if (!strcmp(lastpfx, pfx))
            return lckhome;

        free(lastpfx);
        lastpfx = astrdup(pfx);
    }

    switch (type) {
        case LCKDB_TEMP:
            s = temphome();
            break;
        case LCKDB_PERS:
            s = confhome();
            break;
        default:
            HEDLEY_UNREACHABLE();
            break;
    }

    prefix = vstrcat(2, s, pfx);

    return lckhome = prefix;
}

const char *lckhome(lckdb_t type)
{
    return lckhome_s(type, db_locksprefix);
}

char *lckpath(const char *hash, lckdb_t type, bool mkdir)
{
    const char *prefix;
    char *path;

    assert(hash);

    prefix = lckhome(type);
    if (mkdir)
        rmkdir((char *)prefix, 0755);
    path = vstrcat(2, prefix, hash);

    return path;
}

bool lckdb(const char *hash, lckdb_t type)
{
    char *path;
    bool ret;
    int errnobak = errno;

    path = lckpath(hash, type, true);

    errno = 0;
    ret = !(mkdir(path, 0755) || errno);
    errno = errnobak;

    free(path);

    return ret;
}

bool awaitdb(const char *hash, lckdb_t type)
{
    enum {
        eventsize = sizeof(struct inotify_event),
        buflen = (eventsize + 16) * 1024,
    };

    int length, fd, wd, i;
    char buf[buflen], *path;

    fd = inotify_init();

    if (fd < 0) {
        perror("inotify_init");
        return false;
    }

    path = lckpath(hash, type, false);
    wd = inotify_add_watch(fd, path, IN_DELETE_SELF);
    length = read(fd, buf, buflen);

    if (length < 0) {
        perror("read");
        return false;
    }

    i = 0;
    while (i < length) {
        const struct inotify_event *event = (const struct inotify_event *)&buf[i];
        if (event->len && event->mask & IN_DELETE_SELF)
            goto cleanup;
        i += eventsize + event->len;
    }

cleanup:
    inotify_rm_watch(fd, wd);
    close(fd);
    return true;
}

void alckdb(const char *hash, lckdb_t type)
{
    while (!lckdb(hash, type))
        awaitdb(hash, type);
}

bool ulckdb(const char *hash, lckdb_t type)
{
    char *path;
    bool ret;
    int errnobak = errno;

    path = lckpath(hash, type, false);

    errno = 0;
    ret = !(rmdir(path) || errno);
    errno = errnobak;

    free(path);

    return ret;
}

#endif /* ifndef HEADER_SCRIPTS_C_DATABASE_ */
