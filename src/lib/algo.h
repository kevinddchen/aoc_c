// clang-format Language: C

#pragma once

#include "stdlib.h"

/**
 * Compute greatest common divisor.
 */
int gcd(int a, int b)
{
    a = abs(a);
    b = abs(b);

    while (b != 0) {
        const int temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}
