#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static const char FILENAME[] = "files/day5.txt";

static const int DAY5_PART1_ANS = 607;

/**
 * Struct for keeping track of a fresh ingredient ID range.
 */
typedef struct {
    long first_id;
    long last_id;
} IDRange;

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    // read fresh ingredient id ranges
    char buff[1024] = {};
    // TODO: use dynamic array or linked list for `ranges` instead
    IDRange fresh_id_ranges[256] = {};
    size_t fresh_id_ranges_size = 0;
    while (fgets(buff, sizeof buff, fp) != NULL) {
        // break on empty line
        if (strlen(buff) == 1) {
            assert(buff[0] == '\n');
            break;
        }

        // parse id range
        long first_id = {};
        long last_id = {};
        util_parse_dash_separated_ints(buff, &first_id, &last_id);

        // append range to list
        assert(fresh_id_ranges_size < sizeof(fresh_id_ranges) / sizeof(fresh_id_ranges[0]));
        fresh_id_ranges[fresh_id_ranges_size].first_id = first_id;
        fresh_id_ranges[fresh_id_ranges_size].last_id = last_id;
        fresh_id_ranges_size++;
    }

    // track number of fresh ingredients
    int num_fresh = 0;

    // read available ingredient ids
    while (fgets(buff, sizeof buff, fp) != NULL) {
        const long id = util_atol(buff);

        // check if fresh
        bool fresh = false;
        for (size_t i = 0; i < fresh_id_ranges_size; i++) {
            if (id >= fresh_id_ranges[i].first_id && id <= fresh_id_ranges[i].last_id) {
                fresh = true;
                break;
            }
        }

        if (fresh)
            num_fresh++;
    }

    fclose(fp);

    assert(num_fresh == DAY5_PART1_ANS);

    printf("Day 5\n");
    printf("Part 1: %d\n", num_fresh);
}
