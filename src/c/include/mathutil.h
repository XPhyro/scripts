#ifndef HEADER_SCRIPTS_MATHUTIL
#define HEADER_SCRIPTS_MATHUTIL

#include <stdbool.h>

bool approxzero(double x)
{
    return -1e-12 < x && x < 1e-12;
}

bool approxzerof(double x)
{
    return -1e-7 < x && x < 1e-7;
}

#endif /* ifndef HEADER_SCRIPTS_MATHUTIL */
