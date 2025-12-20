#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char FILENAME[] = "files/day3.txt";

static const long DAY3_PART1_ANS = 17535;
static const long DAY3_PART2_ANS = 173577199527257;

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
 * @param begin Pointer to beginning of string.
 * @param length Number of digits to search through.
 * @returns Pointer to the largest digit.
 */
const char* find_max_digit(const char* begin, size_t length)
{
    const char* max_ptr = begin;
    int max = ctoi(*max_ptr);
    for (size_t i = 1; i < length; i++) {
        const int val = ctoi(begin[i]);
        if (val > max) {
            max_ptr = begin + i;
            max = val;
        }
    }
    return max_ptr;
}

/**
 * Given a bank of batteries, find the largest joltage from turning on `num_batteries` batteries.
 * @param begin Pointer to beginning of string.
 * @param length Number of digits to search through.
 * @param num_batteries Number of batteries to turn on.
 * @returns The largest joltage.
 */
long find_max_joltage(const char* begin, size_t length, int num_batteries)
{
    // for the first digit, we greedily search through the first few digits.
    // once we find the first digit, we greedily search through the next few digits for the second digit.
    // and so forth...
    long joltage = 0;
    size_t searchable_space = length - num_batteries + 1;
    assert(searchable_space > 0);
    for (int i = 0; i < num_batteries; i++) {
        const char* max_digit = find_max_digit(begin, searchable_space);
        joltage = joltage * 10 + ctoi(*max_digit);
        searchable_space -= max_digit - begin;
        begin = max_digit + 1;
    }
    return joltage;
}

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    long sum_joltage_p1 = 0;
    long sum_joltage_p2 = 0;

    // iterate over each line
    char buff[256];
    while (fgets(buff, sizeof buff, fp) != NULL) {
        const size_t length = strlen(buff) - 1;  // ignore newline character

        const long joltage_p1 = find_max_joltage(buff, length, 2);
        sum_joltage_p1 += joltage_p1;

        const long joltage_p2 = find_max_joltage(buff, length, 12);
        sum_joltage_p2 += joltage_p2;
    }
    fclose(fp);

    assert(sum_joltage_p1 == DAY3_PART1_ANS);
    assert(sum_joltage_p2 == DAY3_PART2_ANS);

    printf("Day 3\n");
    printf("Part 1: %ld\n", sum_joltage_p1);
    printf("Part 2: %ld\n", sum_joltage_p2);

    return EXIT_SUCCESS;
}
