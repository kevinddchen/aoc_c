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

void test_util_strtol()
{
    printf("test_util_strtol: ");
    const char str[] = "123,456,789";
    const char* ptr = str;
    assert(util_strtol(str, (char**)&ptr, 0) == 123);
    assert(ptr == str + 3);
    assert(util_strtol(ptr + 1, (char**)&ptr, 0) == 456);
    assert(ptr == str + 7);
    assert(util_strtol(ptr + 1, NULL, 0) == 789);
    assert(ptr == str + 7);  // `ptr` has not changed
    printf("Pass!\n");
}

void test_util_atol()
{
    printf("test_util_atol: ");
    assert(util_atol("123") == 123);
    assert(util_atol("1 2 3") == 1);
    assert(util_atol(" 1 ") == 1);
    printf("Pass!\n");
}

int main()
{
    test_util_max();
    test_util_min();
    test_util_strtol();
    test_util_atol();
}
