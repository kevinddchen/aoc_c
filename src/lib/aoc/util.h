#pragma once

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

/**
 * Same as `atol` stdlib function, but safer.
 */
inline long aoc_util_atol(const char* str) {
    errno = 0;  // reset
    char* end = {};
    const long num = strtol(str, &end, 10);
    assert(end != str);       // check conversion performned
    assert(errno != ERANGE);  // check out-of-range error
    return num;
}
