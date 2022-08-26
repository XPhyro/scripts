#ifndef HEADER_SCRIPTS_STRUTIL
#define HEADER_SCRIPTS_STRUTIL

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>
#include <sys/types.h>

#include <hedley.h>
#include <stdutil.h>

HEDLEY_MALLOC HEDLEY_ALWAYS_INLINE void *amallocset(size_t size, int c)
{
    return memset(amalloc(size), c, size);
}

/* if `n` may be greater than `size`, use `amallocsetn_s(...)`.
 * otherwise this function will likely lead to a segfault.
 */
HEDLEY_MALLOC HEDLEY_ALWAYS_INLINE void *amallocsetn(size_t size, int c, size_t n)
{
    assert(n <= size);
    return memset(amalloc(size), c, n);
}

HEDLEY_MALLOC HEDLEY_ALWAYS_INLINE void *amallocsetn_s(size_t size, int c, size_t n)
{
    return memset(amalloc(size), c, MIN(size, n));
}

HEDLEY_MALLOC char *astrdup(const char *s)
{
    size_t size = strlen(s) * sizeof(char);
    char *dup = amalloc(size);
    memcpy(dup, s, size);
    return dup;
}

char *strend(const char *s)
{
    if (!s)
        return NULL;

    for (; *s; s++) {}
    return (char *)s;
}

char *strnend(const char *s, size_t n)
{
    size_t i;

    if (!s)
        return NULL;

    for (i = 0; i < n && *s; i++, s++) {}
    return (char *)s;
}

HEDLEY_ALWAYS_INLINE bool streq(const char *s1, const char *s2)
{
    return !strcmp(s1, s2);
}

HEDLEY_ALWAYS_INLINE bool strneq(const char *s1, const char *s2, size_t n)
{
    return !strncmp(s1, s2, n);
}

HEDLEY_ALWAYS_INLINE bool strcaseeq(const char *s1, const char *s2)
{
    return !strcasecmp(s1, s2);
}

HEDLEY_ALWAYS_INLINE bool strncaseeq(const char *s1, const char *s2, size_t n)
{
    return !strncasecmp(s1, s2, n);
}

bool strpfx(const char *s, const char *pfx)
{
    char cs, cpfx;

    if (!s)
        return !pfx;
    if (!pfx)
        return false;

    for (;;) {
        cs = *s++;
        cpfx = *pfx++;
        if (!cs)
            return !cpfx;
        if (!cpfx)
            return true;
        if (cs != cpfx)
            return false;
    }
}

bool strnpfx(const char *s, const char *pfx, size_t n)
{
    size_t i;
    char cs, cpfx;

    if (!s)
        return !pfx;
    if (!pfx)
        return false;

    for (i = 0; i < n; i++) {
        cs = *s++;
        cpfx = *pfx++;
        if (!cs)
            return !cpfx;
        if (!cpfx)
            return true;
        if (cs != cpfx)
            return false;
    }

    return true;
}

bool strsfx(const char *s, const char *sfx)
{
    char *send, *sfxend;

    if (!s)
        return !sfx;
    if (!sfx)
        return false;

    send = strend(s);
    sfxend = strend(sfx);

    for (;;) {
        if (*--send != *--sfxend)
            return false;
        if (send == s)
            return sfxend == sfx;
        if (sfxend == sfx)
            return true;
    }
}

bool strnsfx(const char *s, const char *sfx, size_t n)
{
    size_t i;
    char *send, *sfxend;

    if (!s)
        return !sfx;
    if (!sfx)
        return false;

    send = strend(s);
    sfxend = strend(sfx);

    for (i = 0; i < n; i++) {
        if (*--send != *--sfxend)
            return false;
        if (send == s)
            return sfxend == sfx;
        if (sfxend == sfx)
            return true;
    }

    return true;
}

/* BEGIN COMMENT FROM ORIGINAL AUTHOR */
/* Strrstr.c, included for those computers that do not have it. */
/* Written by Kent Irwin, irwin@leland.stanford.edu.  I am
   responsible for bugs */
/* END COMMENT FROM ORIGINAL AUTHOR */
/* the following is not strictly author's code, but
 * rather is a refactored and formatted version of it.
 * there might be bugs that do not exist in the original.
 * source:
 * https://stuff.mit.edu/afs/sipb/user/cordelia/Diplomacy/mapit/strrstr.c */
char *strrstr(const char *haystack, const char *needle)
{
    const char *scn, *psch, *psh;

    if (!haystack || !needle)
        return NULL;
    if (!*needle)
        return (char *)haystack;

    psh = strend(haystack);

    while (psh != haystack) {
        psch = --psh;
        scn = needle;
        for (;;) {
            if (*psch++ != *scn++)
                break;
            if (!*scn)
                return (char *)psh;
        }
    }

    return NULL;
}

char *strrstre(const char *haystack, const char *needle, const char *haystackend)
{
    const char *scn, *psch, *psh;

    if (!haystack || !needle)
        return NULL;
    if (!*needle)
        return (char *)haystack;

    psh = haystackend;

    while (psh != haystack) {
        psch = --psh;
        scn = needle;
        for (;;) {
            if (*psch++ != *scn++)
                break;
            if (!*scn)
                return (char *)psh;
        }
    }

    return NULL;
}

char *strcaserstr(const char *haystack, const char *needle)
{
    const char *scn, *psch, *psh;

    if (!haystack || !needle)
        return NULL;
    if (!*needle)
        return (char *)haystack;

    psh = strend(haystack);

    while (psh != haystack) {
        psch = --psh;
        scn = needle;
        for (;;) {
            if (tolower(*psch++) != tolower(*scn++))
                break;
            if (!*scn)
                return (char *)psh;
        }
    }

    return NULL;
}

char *strcaserstre(const char *haystack, const char *needle, const char *haystackend)
{
    const char *scn, *psch, *psh;

    if (!haystack || !needle)
        return NULL;
    if (!*needle)
        return (char *)haystack;

    psh = haystackend;

    while (psh != haystack) {
        psch = --psh;
        scn = needle;
        for (;;) {
            if (tolower(*psch++) != tolower(*scn++))
                break;
            if (!*scn)
                return (char *)psh;
        }
    }

    return NULL;
}

bool strisfilter(const char *s, int (*func)(int))
{
    int c;

    if (!s)
        return false;

    for (; (c = *s); s++)
        if (!func(c))
            return false;

    return true;
}

bool strnisfilter(const char *s, int (*func)(int), size_t n)
{
    size_t i;

    if (!s)
        return false;

    for (i = 0; i < n; i++)
        if (!func(s[i]))
            return false;

    return true;
}

bool intisfilter(const int *s, int (*func)(int))
{
    int c;

    if (!s)
        return false;

    for (; (c = *s); s++)
        if (!func(c))
            return false;

    return true;
}

bool intnisfilter(const int *s, int (*func)(int), size_t n)
{
    size_t i;

    if (!s)
        return false;

    for (i = 0; i < n; i++)
        if (!func(s[i]))
            return false;

    return true;
}

char *astrncatbuf(char *buf, size_t bufsize, const char **sptr, size_t n, const size_t *sizes,
                  size_t totsize)
{
    size_t i, j, idx, ssize;
    const char *s;

    if (!sptr || !sizes)
        return NULL;

    if (!buf || bufsize < (totsize + 1) * sizeof(char))
        buf = arealloc(buf, (totsize + 1) * sizeof(char));

    for (i = 0, idx = 0; i < n; i++) {
        ssize = sizes[i];
        if (!(s = sptr[i])) {
            free(buf);
            return NULL;
        }
        for (j = 0; j < ssize; buf[idx++] = s[j++]) {}
    }

    buf[totsize] = '\0';

    return buf;
}

HEDLEY_MALLOC char *astrncat(const char **sptr, size_t n, const size_t *sizes, size_t totsize)
{
#ifndef __clang_analyzer__
    return astrncatbuf(amalloc((totsize + 1) * sizeof(char)), totsize + 1, sptr, n, sizes, totsize);
#endif /* ifndef __clang_analyzer__ */
}

HEDLEY_MALLOC char *astrcat(const char **sptr, size_t n)
{
    size_t i, totsize, *sizes;
    char *o;
    const char *s;

    sizes = amalloc(n * sizeof(size_t));
    totsize = 0;

    for (i = 0; i < n; i++) {
        if (!(s = sptr[i])) {
            free(sizes);
            return NULL;
        }
        totsize += (sizes[i] = strlen(s));
    }

    o = astrncat(sptr, n, sizes, totsize);
    free(sizes);

    return o;
}

char *astrcatbuf(char *buf, size_t bufsize, const char **sptr, size_t n)
{
    size_t i, totsize, *sizes;
    char *o;
    const char *s;

    sizes = amalloc(n * sizeof(size_t));
    totsize = 0;

    for (i = 0; i < n; i++) {
        if (!(s = sptr[i])) {
            free(sizes);
            return NULL;
        }
        totsize += (sizes[i] = strlen(s));
    }

    o = astrncatbuf(buf, bufsize, sptr, n, sizes, totsize);
    free(sizes);

    return o;
}

HEDLEY_MALLOC char *vstrncat(size_t n, ...)
{
    va_list ap;
    const char **sptr = amalloc(n * sizeof(char *));
    size_t i, totsize, *sizes;
    char *o;

    sizes = amalloc(n * sizeof(size_t));
    totsize = 0;

    va_start(ap, n);
    for (i = 0; i < n; i++) {
        if (!(sptr[i] = va_arg(ap, char *))) {
            free(sptr);
            free(sizes);
            va_end(ap);
            return NULL;
        }
        totsize += (sizes[i] = va_arg(ap, size_t));
    }
    va_end(ap);

    o = astrncat(sptr, n, sizes, totsize);
    free(sptr);
    free(sizes);

    return o;
}

HEDLEY_MALLOC char *vstrcat(size_t n, ...)
{
    va_list ap;
    const char **sptr = amalloc(n * sizeof(char *));
    size_t i;
    char *o;

    va_start(ap, n);
    for (i = 0; i < n; i++) {
        if (!(sptr[i] = va_arg(ap, char *))) {
            free(sptr);
            va_end(ap);
            return NULL;
        }
    }
    va_end(ap);

    o = astrcat(sptr, n);
    free(sptr);

    return o;
}

char *vstrcatbuf(char *buf, size_t bufsize, size_t n, ...)
{
    va_list ap;
    const char **sptr = amalloc(n * sizeof(char *));
    size_t i;
    char *o;

    va_start(ap, n);
    for (i = 0; i < n; i++) {
        if (!(sptr[i] = va_arg(ap, char *))) {
            free(sptr);
            va_end(ap);
            return NULL;
        }
    }
    va_end(ap);

    o = astrcatbuf(buf, bufsize, sptr, n);
    free(sptr);

    return o;
}

#endif /* ifndef HEADER_SCRIPTS_STRUTIL */
