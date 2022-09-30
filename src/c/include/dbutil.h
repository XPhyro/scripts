#ifndef HEADER_SCRIPTS_C_DBUTIL
#define HEADER_SCRIPTS_C_DBUTIL

#include <assert.h>
#include <errno.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <meta.h>
#include <strutil.h>
#include <sysutil.h>

const char *const db_scriptsprefix = "/scripts/";
const char *const db_locksprefix = "/locks/";

const char *temphome_s(const char *pfx)
{
    static const char *temphome;
    static bool tmphomeset = false;
    char *prefix;

    if (tmphomeset)
        return temphome;

    if (!(prefix = getenv("TMPDIR")))
        prefix = "/tmp";

    tmphomeset = true;
    return temphome = vstrcat(2, prefix, pfx);
}

const char *temphome()
{
    return temphome_s(db_scriptsprefix);
}

const char *confhome_s(const char *pfx)
{
    static const char *confhome;
    static bool confhomeset = false;
    char *prefix, *s;
    bool allocedprefix = false;

    if (confhomeset)
        return confhome;

    if (!(prefix = getenv("XDG_CONFIG_HOME"))) {
        if (!(s = getenv("HOME")) && !(s = getpwuid(getuid())->pw_dir)) {
            confhomeset = true;
            return confhome = NULL;
        }
        prefix = vstrcat(2, s, "/.config");
        allocedprefix = true;
    }

    confhomeset = true;
    confhome = vstrcat(2, prefix, pfx);
    if (allocedprefix)
        free(prefix);
    return confhome;
}

const char *confhome()
{
    return confhome_s(db_scriptsprefix);
}

const char *cachehome_s(const char *pfx)
{
    static const char *cachehome;
    static bool cachehomeset = false;
    char *prefix, *s;
    bool allocedprefix = false;

    if (cachehomeset)
        return cachehome;

    if (!(prefix = getenv("XDG_CACHE_HOME"))) {
        if (!(s = getenv("HOME")) && !(s = getpwuid(getuid())->pw_dir)) {
            cachehomeset = true;
            return cachehome = NULL;
        }
        prefix = vstrcat(2, s, "/.cache");
        allocedprefix = true;
    }

    cachehomeset = true;
    cachehome = vstrcat(2, prefix, pfx);
    if (allocedprefix)
        free(prefix);
    return cachehome;
}

const char *cachehome()
{
    return cachehome_s(db_scriptsprefix);
}

typedef enum { LCKDB_TEMP, LCKDB_PERS } lckdb_t;

const char *lckhome_s(lckdb_t type, const char *pfx)
{
    static const char *lckhome;
    static bool lckhomeset = false;
    char *prefix;
    const char *s;
#ifndef __GNUC__
    s = NULL;
#endif /* !__GNUC__ */

    assert(type == LCKDB_TEMP || type == LCKDB_PERS);

    if (lckhomeset)
        return lckhome;

    switch (type) {
        case LCKDB_TEMP:
            s = temphome();
            break;
        case LCKDB_PERS:
            s = confhome();
            break;
    }

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif /* __GNUC__ */
    prefix = vstrcat(2, s, pfx);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /* __GNUC__ */

    lckhomeset = true;
    return lckhome = prefix;
}

const char *lckhome(lckdb_t type)
{
    return lckhome_s(type, db_locksprefix);
}

char *lckpath(META_NONNULL const char *hash, lckdb_t type, bool mkdir)
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

bool lckdb(META_NONNULL const char *hash, lckdb_t type)
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

bool awaitdb(META_NONNULL const char *hash, lckdb_t type)
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
        struct inotify_event *event = (struct inotify_event *)&buf[i];
        if (event->len && event->mask & IN_DELETE_SELF)
            goto cleanup;
        i += eventsize + event->len;
    }

cleanup:
    inotify_rm_watch(fd, wd);
    close(fd);
    return true;
}

void alckdb(META_NONNULL const char *hash, lckdb_t type)
{
    while (!lckdb(hash, type))
        awaitdb(hash, type);
}

bool ulckdb(META_NONNULL const char *hash, lckdb_t type)
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

#endif /* ifndef HEADER_SCRIPTS_C_DBUTIL */
