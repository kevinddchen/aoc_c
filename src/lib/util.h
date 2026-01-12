#pragma once

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * Same as `strtol` function, but with some error checking.
 */
static inline long util_strtol(const char* str, char** str_end, int base) {
    errno = 0;  // reset
    char* dummy_str_end = {};
    const long num = strtol(str, &dummy_str_end, base);
    assert(dummy_str_end != str);  // check conversion performned
    assert(errno != ERANGE);       // check out-of-range error
    if (str_end != NULL) *str_end = dummy_str_end;
    return num;
}

/**
 * Same as `atol` function, but with some error checking.
 */
static inline long util_atol(const char* str) { return util_strtol(str, NULL, 0); }
