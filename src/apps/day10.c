#include "util.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const char FILENAME[] = "files/day10.txt";

static const long DAY10_PART1_ANS = 479;

/**
 * Parses the bits represented in the indicator light.
 * @param str Pointer to the '[' character.
 * @param str_end Output pointer to the character after the ']'.
 * @returns Unsigned integer representing the bits.
 */
uint32_t parse_indicator_light(const char* str, char** str_end)
{
    str++;
    uint32_t result = 0;
    uint32_t base = 1;
    while (*str != ']') {
        if (*str == '#')
            result += base;
        base *= 2;
        str++;
    }

    if (str_end != NULL)
        *str_end = (char*)str + 1;

    return result;
}

/**
 * Parses the bits represented by the button.
 * @param str Pointer to the '(' character.
 * @param str_end Output pointer to the character after the ')'.
 * @returns Unsigned integer representing the bits.
 */
uint32_t parse_button(const char* str, char** str_end)
{
    uint32_t result = 0;
    while (*str != ')') {
        str++;
        const int power = util_ctoi(*str);
        result += 1 << power;
        str++;
    }

    if (str_end != NULL)
        *str_end = (char*)str + 1;

    return result;
}

/**
 * Compute minimum number of button presses to achieve the given indicator light.
 */
int compute_min_button_presses(uint32_t indicator_light, const Vector* buttons)
{
    if (indicator_light == 0)
        return 0;

    // TODO: use hashset for better performance
    Vector current_combinations = {};
    vector_init(&current_combinations, sizeof(uint32_t));
    vector_push_back(&current_combinations, &indicator_light);

    int num_presses = 0;

    for (size_t n = 0; n < buttons->count; n++) {
        num_presses++;

        Vector next_combinations = {};
        vector_init(&next_combinations, sizeof(uint32_t));
        vector_reserve(&next_combinations, current_combinations.count * buttons->count);

        // iterate over `current_combinations` x `buttons`
        for (size_t i = 0; i < current_combinations.count; i++) {
            for (size_t j = 0; j < buttons->count; j++) {
                const uint32_t curr = ((uint32_t*)current_combinations.items)[i];
                const uint32_t button = ((uint32_t*)buttons->items)[j];
                const uint32_t next = curr ^ button;  // take XOR
                if (next == 0) {
                    vector_free(&current_combinations);
                    vector_free(&next_combinations);
                    return num_presses;
                }
                vector_push_back(&next_combinations, &next);
            }
        }

        // move `next_combinations` into `current_combinations`
        vector_free(&current_combinations);
        current_combinations.items = next_combinations.items;
        current_combinations.count = next_combinations.count;
        current_combinations.capacity = next_combinations.capacity;
    }

    assert(false);  // Unexpected: could not get to indicator light
}

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    int total_button_presses = 0;

    // iterate over each line
    char buff[1024] = {};
    while (fgets(buff, sizeof buff, fp) != NULL) {
        char* ptr = buff;

        // parse indicator light and buttons

        assert(*ptr == '[');
        const uint32_t indicator_light = parse_indicator_light(ptr, &ptr);
        ptr++;

        Vector buttons = {};
        vector_init(&buttons, sizeof(uint32_t));

        while (*ptr == '(') {
            const uint32_t button = parse_button(ptr, &ptr);
            vector_push_back(&buttons, &button);
            ptr++;
        }

        // compute button presses

        const int button_presses = compute_min_button_presses(indicator_light, &buttons);
        total_button_presses += button_presses;

        // cleanup

        vector_free(&buttons);
    }

    printf("Day 10\n");
    printf("Part 1: %d\n", total_button_presses);
    // printf("Part 2: %d\n", );

    assert(total_button_presses == DAY10_PART1_ANS);

    fclose(fp);
    fp = NULL;

    return EXIT_SUCCESS;
}
