#pragma once

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * Same as `atol` function, but with some error checking.
 */
static inline long util_atol(const char* str) {
    errno = 0;  // reset
    char* str_end = {};
    const long num = strtol(str, &str_end, 0);
    assert(str_end != str);   // check conversion performned
    assert(errno != ERANGE);  // check out-of-range error
    return num;
}

/**
 * Parse a string containing two positive integers separated by a dash '-'.
 * @param str String to be parsed.
 * @param first Output first integer.
 * @param second Output second integer.
 */
static inline void util_parse_dash_separated_ints(const char* str, long* first_ptr, long* second_ptr) {
    // find dash separating the two numbers
    const char* dash_ptr = strchr(str, '-');
    assert(dash_ptr != NULL);

    *first_ptr = util_atol(str);
    *second_ptr = util_atol(dash_ptr + 1);
}
