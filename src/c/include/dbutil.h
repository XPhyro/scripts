#ifndef HEADER_SCRIPTS_C_DBUTIL
#define HEADER_SCRIPTS_C_DBUTIL

#include <assert.h>
#include <errno.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <strutil.h>
#include <sysutil.h>

const char *temphome()
{
    static const char *temphome;
    static bool tmphomeset = false;
    char *prefix;

    if (tmphomeset)
        return temphome;

    if (!(prefix = getenv("TMPDIR")))
        prefix = "/tmp";

    tmphomeset = true;
    return temphome = vstrcat(2, prefix, "/scripts");
}

const char *confhome()
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
    confhome = vstrcat(2, prefix, "/scripts");
    if (allocedprefix)
        free(prefix);
    return confhome;
}

const char *cachehome()
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
    cachehome = vstrcat(2, prefix, "/scripts");
    if (allocedprefix)
        free(prefix);
    return cachehome;
}

typedef enum { LCKDB_TEMP, LCKDB_PERS } lckdb_t;

const char *lckhome(lckdb_t type)
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
    prefix = vstrcat(2, s, "/locks/");
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /* __GNUC__ */

    lckhomeset = true;
    return lckhome = prefix;
}

bool lckdb(const char *hash, lckdb_t type)
{
    const char *prefix;
    char *path;
    bool ret;

    assert(hash);

    prefix = lckhome(type);
    rmkdir((char *)prefix, 0755);
    path = vstrcat(2, prefix, hash);

    errno = 0;
    ret = !(mkdir(path, 0755) || errno);
    free(path);
    return ret;
}

bool ulckdb(const char *hash, lckdb_t type)
{
    char *path;
    bool ret;

    assert(hash);

    path = vstrcat(2, lckhome(type), hash);

    errno = 0;
    ret = !(rmdir(path) || errno);
    free(path);
    return ret;
}

#endif /* ifndef HEADER_SCRIPTS_C_DBUTIL */
