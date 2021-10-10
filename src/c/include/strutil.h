#include <stddef.h>
#include <stdbool.h>

char *strend(const char *s)
{
    char *tmp = s;
    for (;;) {
        if (!*tmp)
            break;
        tmp++;
    }
    return tmp;
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
