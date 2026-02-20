#include "util.h"

#include <stdio.h>

void test_util_strtol()
{
    printf("test_util_strtol: ");
    char str[] = "123,456,789";
    char* ptr = str;
    assert(util_strtol(str, &ptr, 0) == 123);
    assert(ptr == str + 3);
    assert(util_strtol(ptr + 1, &ptr, 0) == 456);
    assert(ptr == str + 7);
    assert(util_strtol(ptr + 1, &ptr, 0) == 789);
    assert(ptr == str + 11);

    assert(util_strtol(str, NULL, 0) == 123);
    printf("Pass!\n");
}

void test_util_atol()
{
    printf("test_util_atol: ");
    assert(util_atol("123") == 123);
    assert(util_atol("1 2 3") == 1);
    assert(util_atol(" 1 ") == 1);
    assert(util_atol("\t1\n") == 1);
    printf("Pass!\n");
}

void test_util_ctoi()
{
    printf("test_util_ctoi: ");
    char buff[2];
    for (int i = 0; i < 10; i++) {
        snprintf(buff, sizeof(buff), "%d", i);
        assert(util_ctoi(buff[0]) == i);
    }
    printf("Pass!\n");
}

int main()
{
    test_util_strtol();
    test_util_atol();
    test_util_ctoi();
    return EXIT_SUCCESS;
}
