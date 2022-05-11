#include <stdio.h>

#include <stdutil.h>

typedef unsigned long long int llu;

llu gcd(llu a, llu b)
{
    return b == 0 ? a : a > b ? gcd(a - b, b) : gcd(a, b - a);
}

int main(int argc, char *argv[])
{
    const char *const err = "invalid argument";
    llu a, b;
    int i;

    for (i = 1; i < argc - 1; i++) {
        a = astrtoull(argv[i], err);
        b = astrtoull(argv[++i], err);
        printf("gcd(%llu, %llu) = %llu", a, b, gcd(a, b));
    }

    return 0;
}
