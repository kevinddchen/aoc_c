#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char FILENAME[] = "files/day3.txt";

static const long DAY3_PART1_ANS = 17535;

/**
 * Convert a `char` digit to integer.
 */
int ctoi(char c)
{
    const int i = c - '0';
    assert(i >= 0);
    assert(i < 10);
    return i;
}

/**
 * Find the largest digit in a string of digits. For duplicates, will always return the left-most digit.
 * @param start Pointer to start of string.
 * @param length Number of digits to search through.
 */
const char* find_max_digit(const char* start, size_t length)
{
    const char* max_ptr = start;
    int max = ctoi(*max_ptr);
    for (size_t i = 1; i < length; i++) {
        const int val = ctoi(start[i]);
        if (val > max) {
            max_ptr = start + i;
            max = val;
        }
    }
    return max_ptr;
}

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    long sum_joltage = 0;

    // iterate over each line
    char buff[256];
    while (fgets(buff, sizeof buff, fp) != NULL) {
        const size_t length = strlen(buff) - 1;  // ignore newline character

        // first digit can be any except for last
        const char* first_digit = find_max_digit(buff, length - 1);

        // second digit must be after the first
        const size_t remaining = length - (first_digit - buff + 1);
        assert(remaining != 0);
        assert(remaining < length);
        const char* second_digit = find_max_digit(first_digit + 1, remaining);

        const long joltage = 10 * ctoi(*first_digit) + ctoi(*second_digit);
        sum_joltage += joltage;
    }
    fclose(fp);

    assert(sum_joltage == DAY3_PART1_ANS);

    printf("Day 3\n");
    printf("Part 1: %ld\n", sum_joltage);

    return EXIT_SUCCESS;
}
