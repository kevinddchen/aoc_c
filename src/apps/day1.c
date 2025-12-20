#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const int NUM_POSITIONS = 100;

static const char FILENAME[] = "files/day1.txt";

static const int DAY1_PART1_ANS = 1078;
static const int DAY1_PART2_ANS = 6412;

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    int position = 50;  // current dial position
    int stopped_on_zero = 0;
    int passed_by_zero = 0;

    // iterate over each line
    char buff[64];
    while (fgets(buff, sizeof buff, fp) != NULL) {
        const char direction = buff[0];
        const int num_clicks = atoi(buff + 1);
        if (num_clicks == 0)
            continue;
        const int prev_position = position;

        // execute rotation
        if (direction == 'R')
            position += num_clicks;
        else if (direction == 'L')
            position -= num_clicks;
        else
            assert(false);

        // compute div and mod, truncated to -inf
        div_t result = div(position, NUM_POSITIONS);
        if (result.rem < 0) {
            result.rem += NUM_POSITIONS;
            result.quot--;
        }
        position = result.rem;

        if (position == 0)
            stopped_on_zero++;

        passed_by_zero += abs(result.quot);
        // two corner cases:
        // (1) if going left and ending position is 0, should add a pass by zero
        // (2) if starting from 0 and going left, should subtract a pass by zero
        if (direction == 'L') {
            if (prev_position == 0)
                passed_by_zero--;
            if (position == 0)
                passed_by_zero++;
        }
    }
    fclose(fp);

    assert(stopped_on_zero == DAY1_PART1_ANS);
    assert(passed_by_zero == DAY1_PART2_ANS);

    printf("Day 1\n");
    printf("Part 1: %d\n", stopped_on_zero);
    printf("Part 2: %d\n", passed_by_zero);

    return EXIT_SUCCESS;
}
