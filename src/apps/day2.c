#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static const char FILENAME[] = "files/day2.txt";


/**
 * Count the number of digits in `x`.
 */
int count_digits(long x)
{
    int count = 0;
    while (x > 0) {
        x /= 10;
        count++;
    }
    return count;
}

/**
 * Check if x equals a sequence of digits repeated twice.
 */
bool is_repeating(long x)
{
    const int num_digits = count_digits(x);
    if (num_digits % 2 == 1)
        return false;

    const int length = num_digits / 2;

    long unit = 1;
    long repeating = 0;
    for (int i = 0; i < length; i++) {
        const long digit = x % 10;
        repeating += unit * digit;
        unit *= 10;
        x /= 10;
    }
    return x == repeating;
}


int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    // copy everything to buffer
    char buff[1024];
    fgets(buff, sizeof buff, fp);

    // tracks sum of all invalid IDs
    long sum_invalid_ids = 0;

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
            if (is_repeating(x))
                sum_invalid_ids += x;
        }

        // continue to next comma-separated pair...
        pair = strtok(NULL, ",");
    }
    fclose(fp);

    printf("Day 2\n");
    printf("Part 1: %ld\n", sum_invalid_ids);

    return EXIT_SUCCESS;
}
