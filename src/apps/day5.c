#include "util.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
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

    char buff[1024] = {};

    // read fresh ingredient id ranges
    Vector fresh_id_ranges = {};
    vector_init(&fresh_id_ranges, sizeof(IDRange));
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
        const IDRange range = {first_id, last_id};
        vector_push_back(&fresh_id_ranges, &range);
    }

    IDRange* fresh_id_ranges_items = fresh_id_ranges.items;

    // track number of fresh ingredients
    int num_fresh = 0;

    // read available ingredient ids
    while (fgets(buff, sizeof buff, fp) != NULL) {
        const long id = util_atol(buff);

        // check if fresh
        bool fresh = false;
        for (size_t i = 0; i < fresh_id_ranges.count; i++) {
            if (id >= fresh_id_ranges_items[i].first_id && id <= fresh_id_ranges_items[i].last_id) {
                fresh = true;
                break;
            }
        }

        if (fresh)
            num_fresh++;
    }

    fclose(fp);
    fp = NULL;

    vector_free(&fresh_id_ranges);
    fresh_id_ranges_items = NULL;

    assert(num_fresh == DAY5_PART1_ANS);

    printf("Day 5\n");
    printf("Part 1: %d\n", num_fresh);
}
