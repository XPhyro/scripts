#include <stddef.h>
#include <stdbool.h>

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
