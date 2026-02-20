// clang-format Language: C

#pragma once

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

/**
 * Same as `strtol` function, but with some error checking.
 */
static inline long util_strtol(const char* str, char** str_end, int base)
{
    errno = 0;  // reset
    char* dummy_str_end = {};
    const long num = strtol(str, &dummy_str_end, base);
    assert(dummy_str_end != str);  // check conversion performned
    assert(errno != ERANGE);       // check out-of-range error
    if (str_end != NULL)
        *str_end = dummy_str_end;
    return num;
}

/**
 * Same as `atol` function, but with some error checking.
 */
static inline long util_atol(const char* str)
{
    return util_strtol(str, NULL, 0);
}

/**
 * Convert a `char` digit to integer.
 */
static inline int util_ctoi(char c)
{
    const int i = c - '0';
    assert(0 <= i);
    assert(i < 10);
    return i;
}
