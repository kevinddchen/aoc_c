#include "io.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const char FILENAME[] = "files/day7.txt";

static const int DAY7_PART1_ANS = 1560;
static const long DAY7_PART2_ANS = 25592971184998;

/**
 * Extend the beams downwards by one line.
 * @param curr_line Current line.
 * @param next_line Line below the current line, will be modified by inserting '|' characters.
 * @param line_length Length of a line, in number of characters.
 */
int extend_beams(const char* curr_line, char* next_line, size_t line_length)
{
    int num_splits = 0;
    for (size_t i = 0; i < line_length; i++) {
        const char curr_char = curr_line[i];
        const char next_char = next_line[i];
        if (curr_char != '|' && curr_char != 'S')
            continue;
        // extend beam
        if (next_char == '^') {
            // split beam
            num_splits++;
            assert(i > 0 && i < line_length - 1);
            next_line[i - 1] = '|';
            next_line[i + 1] = '|';
        } else {
            // continue beam
            next_line[i] = '|';
        }
    }
    return num_splits;
}

/**
 * Backtrack up by one line, updating `curr_timelines` to the appropriate values.
 * @param curr_line Current line.
 * @param prev_line Line above the current line.
 * @param curr_timelines Current timelines associated with each character of the current line. Will be updated with the
 * appropriate values for the previous line.
 * @param line_length Length of a line, in number of characters.
 */
void backtrack(const char* curr_line, const char* prev_line, long* curr_timelines, size_t line_length)
{
    long* prev_timelines = calloc(line_length, sizeof(long));
    assert(prev_timelines != NULL);

    for (size_t i = 0; i < line_length; i++) {
        const char prev_char = prev_line[i];
        const char curr_char = curr_line[i];
        if (prev_char != '|' && prev_char != 'S')
            continue;
        if (curr_char == '^') {
            // add up number of timelines from both sides of the split
            assert(i > 0 && i < line_length - 1);
            prev_timelines[i] = curr_timelines[i - 1] + curr_timelines[i + 1];
        } else {
            // timeline continues
            prev_timelines[i] = curr_timelines[i];
        }
    }
    memcpy(curr_timelines, prev_timelines, line_length * sizeof(long));

    free(prev_timelines);
    prev_timelines = NULL;
}

int main()
{
    Vector lines = {};
    io_readlines(FILENAME, 1024, &lines);
    char** lines_items = lines.items;

    // check all lines have same length
    assert(lines.count > 0);
    const size_t line_length = strlen(lines_items[0]);
    for (size_t i = 1; i < lines.count; i++)
        assert(strlen(lines_items[i]) == line_length);

    // forward pass: we simply step through the diagram downwards, updating the characters as we go to indicate the
    // beams path, and counting the number of splits.

    int num_splits = 0;
    for (size_t i = 0; i < lines.count - 1; i++)
        num_splits += extend_beams(lines_items[i], lines_items[i + 1], line_length);

    // backward pass: we use dynamic programming to compute the answer. Starting at the bottom, we track the number of
    // timelines associated with each beam. At the bottom, each beam corresponds to 1 timeline. As we step upwards, when
    // we come across a '^' spliter, the number of timelines equals the sum of the number of timelines to the left and
    // right of the splitter.

    long* timelines = calloc(line_length, sizeof(long));
    assert(timelines != NULL);

    // initialize values based on last line
    for (size_t i = 0; i < line_length; i++) {
        if (lines_items[lines.count - 1][i] == '|')
            timelines[i] = 1;
    }

    for (size_t i = lines.count - 1; i > 0; i--)
        backtrack(lines_items[i], lines_items[i - 1], timelines, line_length);

    // obtain the answer
    long num_timelines = -1;
    for (size_t i = 0; i < line_length; i++) {
        if (lines_items[0][i] == 'S') {
            num_timelines = timelines[i];
            break;
        }
    }

    printf("Day 7\n");
    printf("Part 1: %d\n", num_splits);
    printf("Part 2: %ld\n", num_timelines);

    assert(num_splits == DAY7_PART1_ANS);
    assert(num_timelines == DAY7_PART2_ANS);

    free(timelines);
    timelines = NULL;
    vector_free_arrays(&lines);

    return EXIT_SUCCESS;
}
