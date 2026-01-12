#include "io.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const char FILENAME[] = "files/day7.txt";

static const int DAY7_PART1_ANS = 1560;

int step_beams(const char* prev_line, char* curr_line, size_t line_length)
{
    int num_splits = 0;
    for (size_t i = 0; i < line_length; i++) {
        const char prev_char = prev_line[i];
        const char curr_char = curr_line[i];
        if (prev_char != 'S' && prev_char != '|')
            continue;
        // extend beam
        if (curr_char == '^') {
            // split beam
            num_splits++;
            if (i > 0)
                curr_line[i - 1] = '|';
            if (i < line_length - 1)
                curr_line[i + 1] = '|';
        } else
            curr_line[i] = '|';
    }
    return num_splits;
}

int main()
{
    Vector lines = {};
    io_readlines(FILENAME, 1024, &lines);

    // check all lines have same length
    assert(lines.count > 0);
    const size_t line_length = strlen(((char**)lines.items)[0]);
    for (size_t i = 1; i < lines.count; i++)
        assert(strlen(((char**)lines.items)[i]) == line_length);

    int num_splits = 0;
    for (size_t i = 1; i < lines.count; i++)
        num_splits += step_beams(((char**)lines.items)[i - 1], ((char**)lines.items)[i], line_length);

    printf("Day 7\n");
    printf("Part 1: %d\n", num_splits);

    assert(num_splits == DAY7_PART1_ANS);

    io_free_lines(&lines);

    return EXIT_SUCCESS;
}
