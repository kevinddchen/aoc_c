#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char FILENAME[] = "files/day2.txt";

/**
 * Return the number of digits in `x`.
 */
int count_digits(long x)
{
    int num_digits = 0;
    while (x > 0) {
        x /= 10;
        num_digits++;
    }
    return num_digits;
}

/**
 * Strip off the left-most `num_digits` digits from `x`.
 * @param x Will be modified by this function.
 * @param num_digits Number of digits.
 * @returns Returns digits stripped from `x`.
 */
long strip_left(long* x, int num_digits)
{
    long power = 1;
    long digits = 0;
    for (int i = 0; i < num_digits; i++) {
        const long digit = *x % 10;
        digits += power * digit;
        power *= 10;
        *x /= 10;
    }
    return digits;
}

/**
 * Check if `x` equals a sequence of digits repeated more than once.
 * @param x
 * @param max_num_repeats Will check for sequences that repeat 2, 3, ..., max_num_repeats times. If negative, will
 * default to equaling the number of digits in `x`.
 */
bool is_repeating(long x, int max_num_repeats)
{
    const int x_length = count_digits(x);
    if (max_num_repeats < 0)
        max_num_repeats = x_length;

    // check all values of `num_repeats`
    for (int num_repeats = 2; num_repeats <= max_num_repeats; num_repeats++) {
        if (x_length % num_repeats != 0)
            continue;

        const int num_digits = x_length / num_repeats;
        long x_copy = x;

        // check if `x` is made of repeating digits
        bool repeating = true;
        const long digits = strip_left(&x_copy, num_digits);
        for (int i = 1; i < num_repeats; i++) {
            if (digits != strip_left(&x_copy, num_digits)) {
                repeating = false;
                break;
            }
        }

        if (repeating) {
            assert(x_copy == 0);
            return true;
        }
    }

    return false;
}

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    // copy everything to buffer
    char buff[1024];
    fgets(buff, sizeof buff, fp);

    // tracks sum of all invalid IDs
    long sum_invalid_ids_p1 = 0;
    long sum_invalid_ids_p2 = 0;

    // iterate over all comma-separated pairs
    char* pair = strtok(buff, ",");
    while (pair != NULL) {
        // find dash separating the two numbers
        char* dash_ptr = strchr(pair, '-');
        assert(dash_ptr != NULL);
        *dash_ptr = '\0';  // turn dash into null char, so `pair` becomes two null-terminated strings

        char* first_str = pair;
        char* second_str = dash_ptr + 1;

        const long first = atol(first_str);
        const long second = atol(second_str);

        for (long x = first; x <= second; x++) {
            if (is_repeating(x, 2))
                sum_invalid_ids_p1 += x;
            if (is_repeating(x, -1))
                sum_invalid_ids_p2 += x;
        }

        // continue to next comma-separated pair...
        pair = strtok(NULL, ",");
    }
    fclose(fp);

    printf("Day 2\n");
    printf("Part 1: %ld\n", sum_invalid_ids_p1);
    printf("Part 2: %ld\n", sum_invalid_ids_p2);

    return EXIT_SUCCESS;
}
