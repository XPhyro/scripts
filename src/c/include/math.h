#ifndef HEADER_SCRIPTS_C_MATH_
#define HEADER_SCRIPTS_C_MATH_

#include <stdbool.h>

bool approxzero(double x)
{
    return -1e-12 < x && x < 1e-12;
}

bool approxzerof(float x)
{
    return -1e-6f < x && x < 1e-6f;
}

bool approxeq(double x, double y)
{
    return approxzero(x - y);
}

bool approxeqf(float x, float y)
{
    return approxzerof(x - y);
}

#endif /* ifndef HEADER_SCRIPTS_C_MATH_ */
