#ifndef HEADER_SCRIPTS_C_MATH_
#define HEADER_SCRIPTS_C_MATH_

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include <hedley.h>

int popcountu(unsigned int x)
{
    int popcount;

#if defined(__GNUC__) || defined(__clang__)
    popcount = __builtin_popcount(x & 0xff);
#else
    int i;
    for (i = 0, popcount = 0; i < sizeof(x) * 8; popcount += (x & (1 << i++)) >> 0) {}
#endif

    return popcount;
}

int64_t ipow64(int64_t base, uint8_t exp)
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
            HEDLEY_FALL_THROUGH;
        case 5:
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
            HEDLEY_FALL_THROUGH;
        case 4:
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
            HEDLEY_FALL_THROUGH;
        case 3:
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
            HEDLEY_FALL_THROUGH;
        case 2:
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
            HEDLEY_FALL_THROUGH;
        case 1:
            if (exp & 1)
                result *= base;
            HEDLEY_FALL_THROUGH;
        default:
            return result;
    }
}

int digitcount32(int32_t x)
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

int digitcountu32(uint32_t x)
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

int digitcount64(int64_t x)
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

int digitcountu64(uint64_t x)
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
