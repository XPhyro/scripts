#ifndef _HEADER_SCRIPTS_STRUTIL
#define _HEADER_SCRIPTS_STRUTIL

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>

#include <stdutil.h>

inline void __attribute__((always_inline)) * mallocset(size_t size, int c)
{
    return memset(amalloc(size), c, size);
}

inline void __attribute__((always_inline)) * mallocsetn(size_t size, int c, size_t n)
{
    /* it is the user's responsibility to ensure n <= size */
    return memset(amalloc(size), c, n);
}

inline void __attribute__((always_inline)) * mallocsetn_s(size_t size, int c, size_t n)
{
    return memset(amalloc(size), c, MIN(size, n));
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

inline bool __attribute__((always_inline)) streq(const char *s1, const char *s2)
{
    return !strcmp(s1, s2);
}

inline bool __attribute__((always_inline)) strneq(const char *s1, const char *s2, size_t n)
{
    return !strncmp(s1, s2, n);
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

    for (; (c = *s); s++)
        if (!func(c))
            return false;

    return true;
}

bool strnisfilter(const char *s, int (*func)(int), size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        if (!func(s[i]))
            return false;

    return true;
}

bool intisfilter(const int *s, int (*func)(int))
{
    int c;

    for (; (c = *s); s++)
        if (!func(c))
            return false;

    return true;
}

bool intnisfilter(const int *s, int (*func)(int), size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        if (!func(s[i]))
            return false;

    return true;
}

#endif /* ifndef _HEADER_SCRIPTS_STRUTIL */
