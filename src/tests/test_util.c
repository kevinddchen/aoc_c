#include "util.h"

#include <assert.h>
#include <stdio.h>

void test_util_max()
{
    printf("test_util_max: ");
    assert(MAX(0, 1) == 1);
    assert(MAX(-1, 1) == 1);
    assert(MAX(123, 99) == 123);
    printf("Pass!\n");
}

void test_util_min()
{
    printf("test_util_min: ");
    assert(MIN(0, 1) == 0);
    assert(MIN(-1, 1) == -1);
    assert(MIN(123, 99) == 99);
    printf("Pass!\n");
}

void test_util_atol()
{
    printf("test_util_atol: ");
    assert(util_atol("123") == 123);
    assert(util_atol("1 2 3") == 1);
    printf("Pass!\n");
}

void test_util_parse_dash_separated_ints()
{
    printf("test_util_parse_dash_separated_ints: ");
    long first = {};
    long second = {};
    util_parse_dash_separated_ints("123-99", &first, &second);
    assert(first == 123);
    assert(second == 99);
    printf("Pass!\n");
}

int main()
{
    test_util_max();
    test_util_min();
    test_util_atol();
    test_util_parse_dash_separated_ints();
}
