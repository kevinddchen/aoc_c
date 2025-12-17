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
 * Computes the integer exponent base^exp.
 */
long ipow(long base, int exp)
{
    long result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = result * base;
        }
        base *= base;
        exp /= 2;
    }
    return result;
}

int main()
{
    FILE* const fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    // copy everything to buffer
    char buff[1024];
    fgets(buff, sizeof buff, fp);

    // tracks sum of all invalid IDs
    long sum_invalid_ids = 0;

    // iterate over all comma-separated pairs
    char* pair = strtok(buff, ",");
    while (pair != NULL) {
        // figure out first and second numbers
        char* const dash_ptr = strchr(pair, '-');
        char* const first = pair;
        char* const second = dash_ptr + 1;
        *dash_ptr = '\0';  // turn dash into null char, so `first` becomes a null-terminated string

        const long first_num = atol(first);
        const long second_num = atol(second);

        // take first half of `first`. this will be used as the initial repeating digits
        const int init_num_digits = (dash_ptr - first) / 2;
        char temp[64];
        strncpy(temp, first, init_num_digits);
        temp[init_num_digits] = '\0';

        // iterate over all possible repeating digits
        for (long repeating_digits = atol(temp);; repeating_digits++) {
            const int num_digits = count_digits(repeating_digits);
            const long invalid_id = repeating_digits * (ipow(10, num_digits) + 1);

            // check id is within range given by `first` and `second`
            if (invalid_id < first_num)
                continue;
            if (invalid_id > second_num)
                break;

            sum_invalid_ids += invalid_id;
        }

        // continue to next comma-separated pair...
        pair = strtok(NULL, ",");
    }
    fclose(fp);

    printf("Day 2\n");
    printf("Part 1: %ld\n", sum_invalid_ids);

    return EXIT_SUCCESS;
}
