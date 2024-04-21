#ifndef HEADER_SCRIPTS_C_MATH_
#define HEADER_SCRIPTS_C_MATH_

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

int64_t ipow(int64_t base, uint8_t exp)
// https://gist.github.com/orlp/3551590
{
    // anything past 63 is a guaranteed overflow with base > 1
    static const uint8_t highest_bit_set[] = {
        0,   1,   2,   2,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   5,   5,
        5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,   6,
        6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
        6,   6,   6,   6,   6,   6,   6,   6,   6,   255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255,
    };

    int64_t result = 1;

    switch (highest_bit_set[exp]) {
        case 255: // we use 255 as an overflow marker and return 0 on overflow/underflow
            if (base == 1) {
                return 1;
            }

            if (base == -1) {
                return 1 - 2 * (exp & 1);
            }

            return 0;
        case 6:
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
        case 5:
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
        case 4:
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
        case 3:
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
        case 2:
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
        case 1:
            if (exp & 1)
                result *= base;
        default:
            return result;
    }
}

_Static_assert(sizeof(int) == 4, "int must be 4 bytes");
int digitcount(int x)
{
    if (x < 0)
        x = (x == INT_MIN) ? INT_MAX : -x;
    if (x < 10)
        return 1;
    if (x < 100)
        return 2;
    if (x < 1000)
        return 3;
    if (x < 10000)
        return 4;
    if (x < 100000)
        return 5;
    if (x < 1000000)
        return 6;
    if (x < 10000000)
        return 7;
    if (x < 100000000)
        return 8;
    if (x < 1000000000)
        return 9;
    return 10;
}

int digitcountu(unsigned int x)
{
    if (x < 10)
        return 1;
    if (x < 100)
        return 2;
    if (x < 1000)
        return 3;
    if (x < 10000)
        return 4;
    if (x < 100000)
        return 5;
    if (x < 1000000)
        return 6;
    if (x < 10000000)
        return 7;
    if (x < 100000000)
        return 8;
    if (x < 1000000000)
        return 9;
    return 10;
}

_Static_assert(sizeof(long int) == 8, "long int must be 8 bytes");
int digitcountl(long int x)
{
    if (x < 0)
        x = (x == INT_MIN) ? INT_MAX : -x;
    if (x < 10)
        return 1;
    if (x < 100)
        return 2;
    if (x < 1000)
        return 3;
    if (x < 10000)
        return 4;
    if (x < 100000)
        return 5;
    if (x < 1000000)
        return 6;
    if (x < 10000000)
        return 7;
    if (x < 100000000)
        return 8;
    if (x < 1000000000)
        return 9;
    if (x < 10000000000)
        return 10;
    if (x < 100000000000)
        return 11;
    if (x < 1000000000000)
        return 12;
    if (x < 10000000000000)
        return 13;
    if (x < 100000000000000)
        return 14;
    if (x < 1000000000000000)
        return 15;
    if (x < 10000000000000000)
        return 16;
    if (x < 100000000000000000)
        return 17;
    if (x < 1000000000000000000)
        return 18;
    return 19;
}

int digitcountul(unsigned long int x)
{
    if (x < 10)
        return 1;
    if (x < 100)
        return 2;
    if (x < 1000)
        return 3;
    if (x < 10000)
        return 4;
    if (x < 100000)
        return 5;
    if (x < 1000000)
        return 6;
    if (x < 10000000)
        return 7;
    if (x < 100000000)
        return 8;
    if (x < 1000000000)
        return 9;
    if (x < 10000000000)
        return 10;
    if (x < 100000000000)
        return 11;
    if (x < 1000000000000)
        return 12;
    if (x < 10000000000000)
        return 13;
    if (x < 100000000000000)
        return 14;
    if (x < 1000000000000000)
        return 15;
    if (x < 10000000000000000)
        return 16;
    if (x < 100000000000000000)
        return 17;
    if (x < 1000000000000000000)
        return 18;
    return 19;
}

_Static_assert(sizeof(long long int) == sizeof(long int),
               "long long int must have the same size as long int");
int digitcountll(unsigned long long int x)
{
    return digitcountl(x);
}

int digitcountull(unsigned long long int x)
{
    return digitcountul(x);
}

int sign(int x)
{
    return (x > 0) - (x < 0);
}

long int signl(long int x)
{
    return (x > 0) - (x < 0);
}

long long int signll(long long int x)
{
    return (x > 0) - (x < 0);
}

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
