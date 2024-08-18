#ifndef HEADER_SCRIPTS_C_UNITS_
#define HEADER_SCRIPTS_C_UNITS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "param.h"
#include "stdlib.h"

static const char *units[] = {
    "y", "mo", "w", "d", "h", "m", "s", "ms", "us", "ns",
};

static const unsigned long long durations[ARRLEN(units)] = {
    31556952000000000ull,
    2629746000000000ull,
    604800000000000ull,
    86400000000000ull,
    3600000000000ull,
    60000000000ull,
    1000000000ull,
    1000000ull,
    1000ull,
    1ull,
};

char *tick_to_human(unsigned long long tick)
{
    enum {
        max_len = 256,
    };

    if (!tick) {
        char *result = amalloc(3);
        strcpy(result, "0s");
        return result;
    }

    char *result = amalloc(max_len);
    *result = '\0';

    char buffer[max_len];
    for (int i = 0; i < ARRLEN(units); ++i) {
        if (tick < durations[i]) {
            continue;
        }

        snprintf(buffer, sizeof(buffer), "%llu%s", tick / durations[i], units[i]);
        strncat(result, buffer, max_len);

        tick %= durations[i];
    }

    return result;
}

#endif /* ifndef HEADER_SCRIPTS_C_UNITS_ */
