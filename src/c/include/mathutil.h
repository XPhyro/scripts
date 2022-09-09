#ifndef HEADER_SCRIPTS_C_MATHUTIL
#define HEADER_SCRIPTS_C_MATHUTIL

#include <stdbool.h>

bool approxzero(double x)
{
    return -1e-12 < x && x < 1e-12;
}

bool approxzerof(float x)
{
    return -1e-7f < x && x < 1e-7f;
}

#endif /* ifndef HEADER_SCRIPTS_C_MATHUTIL */
