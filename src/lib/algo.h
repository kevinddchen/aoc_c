// clang-format Language: C

#pragma once

#include "stdlib.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * Compute greatest common divisor.
 */
long gcd(long a, long b)
{
    a = labs(a);
    b = labs(b);

    while (b != 0) {
        const long temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}
