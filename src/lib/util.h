#pragma once

/**
 * Same as `atol` function, but with some error checking.
 */
long util_atol(const char* str);

/**
 * Parse a string containing two positive integers separated by a dash '-'.
 * @param str String to be parsed.
 * @param first Output first integer.
 * @param second Output second integer.
 */
void util_parse_dash_separated_ints(const char* str, long* first_ptr, long* second_ptr);
