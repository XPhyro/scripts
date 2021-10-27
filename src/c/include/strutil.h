#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

char *strend(const char *s)
{
    for (; *s; s++);
    return (char *)s;
}

bool streq(const char *s1, const char *s2)
{
    char c1, c2;

    if (!s1)
        return !s2;

    for (;;) {
        c1 = *s1++;
        c2 = *s2++;
        if (!c1)
            return !c2;
        if (c1 != c2)
            return false;
    }
}

bool strneq(const char *s1, const char *s2, size_t n)
{
    size_t i;
    char c1, c2;

    if (!s1)
        return !s2;

    for (i = 0; i < n; i++) {
        c1 = *s1++;
        c2 = *s2++;
        if (!c1)
            return !c2;
        if (c1 != c2)
            return false;
    }

    return true;
}

bool strpfx(const char *s, const char *pfx)
{
    char cs, cpfx;

    if (!s)
        return !pfx;

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
 * rather is a refactored and formatted version of it
 * source:
 * https://stuff.mit.edu/afs/sipb/user/cordelia/Diplomacy/mapit/strrstr.c */
char *strrstr(const char *haystack, const char *needle)
{
    const char *scn, *psch, *psh;

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
