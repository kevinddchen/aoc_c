#include "algo.h"

#include <assert.h>
#include <stdio.h>

void test_algo_gcd()
{
    printf("test_algo_gcd: ");
    assert(gcd(12, 8) == 4);
    assert(gcd(123, 99) == 3);
    assert(gcd(17, 23) == 1);
    assert(gcd(100, 0) == 100);
    assert(gcd(0, 100) == 100);
    assert(gcd(100, 1) == 1);
    assert(gcd(1, 100) == 1);
    assert(gcd(100, 100) == 100);

    const long p1 = 100003;
    const long p2 = 100019;
    const long p3 = 100043;
    assert(gcd(p1 * p2, p1 * p3) == p1);

    printf("Pass!\n");
}

void test_algo_max()
{
    printf("test_algo_max: ");
    assert(MAX(0, 1) == 1);
    assert(MAX(-1, 1) == 1);
    assert(MAX(123, 99) == 123);
    printf("Pass!\n");
}

void test_algo_min()
{
    printf("test_algo_min: ");
    assert(MIN(0, 1) == 0);
    assert(MIN(-1, 1) == -1);
    assert(MIN(123, 99) == 99);
    printf("Pass!\n");
}

int main()
{
    test_algo_gcd();
    test_algo_max();
    test_algo_min();
    return EXIT_SUCCESS;
}
