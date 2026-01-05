#include "util.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char FILENAME[] = "files/day5.txt";

static const int DAY5_PART1_ANS = 607;
static const long DAY5_PART2_ANS = 342433357244012;

/**
 * Struct for keeping track of a fresh ingredient ID range.
 */
typedef struct {
    long first_id;
    long last_id;
} IDRange;

/**
 * Given a list of ranges, we construct a list of disjoint ranges.
 * @param Input ranges.
 * @param Output disjoint ranges.
 */
void construct_disjoint_ranges(const Vector* ranges, Vector* disjoint_ranges)
{
    vector_init(disjoint_ranges, sizeof(IDRange));

    // the strategy is to use `disjoint_ranges` to keep track of the current list of disjoint ranges, and iterate
    // through each element of `ranges` and trying to add to `disjoint_ranges`. The only tricky thing is comparing
    // ranges if they overlap, in which case we should include their union to the list of disjoint ranges.

    const IDRange* ranges_items = ranges->items;
    for (size_t i = 0; i < ranges->count; i++) {
        // copy existing range
        IDRange range = {};
        range.first_id = ranges_items[i].first_id;
        range.last_id = ranges_items[i].last_id;

        Vector new_disjoint_ranges = {};
        vector_init(&new_disjoint_ranges, sizeof(IDRange));
        // TODO: reserve size `disjoint_ranges->count`

        // compare to existing disjoint ranges
        const IDRange* disjoint_ranges_items = disjoint_ranges->items;
        for (size_t j = 0; j < disjoint_ranges->count; j++) {
            if ((disjoint_ranges_items[j].last_id < range.first_id) || range.last_id < disjoint_ranges_items[j].first_id) {
                // if ranges are disjoint, we keep the jth element
                vector_push_back(&new_disjoint_ranges, &disjoint_ranges_items[j]);
            } else {
                // otherwise, drop jth element but update `range` with larger limits
                range.first_id = MIN(disjoint_ranges_items[j].first_id, range.first_id);
                range.last_id = MAX(disjoint_ranges_items[j].last_id, range.last_id);
            }
        }
        vector_push_back(&new_disjoint_ranges, &range);

        // update `disjoint_ranges`
        free(disjoint_ranges->items);
        disjoint_ranges->items = new_disjoint_ranges.items;
        disjoint_ranges->count = new_disjoint_ranges.count;
        disjoint_ranges->capacity = new_disjoint_ranges.capacity;
        // NOTE: we moved `new_disjoint_ranges`, so we don't need to deallocate it
    }
}

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
        IDRange range = {};
        util_parse_dash_separated_ints(buff, &range.first_id, &range.last_id);

        vector_push_back(&fresh_id_ranges, &range);
    }

    IDRange* fresh_id_ranges_items = fresh_id_ranges.items;

    // === PART 1 =============================================================

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

    // === PART 2 =============================================================

    Vector disjoint_id_ranges = {};
    construct_disjoint_ranges(&fresh_id_ranges, &disjoint_id_ranges);

    long num_all_fresh = 0;

    const IDRange* disjoint_id_ranges_items = disjoint_id_ranges.items;
    for (size_t i = 0; i < disjoint_id_ranges.count; i++) {
        num_all_fresh += disjoint_id_ranges_items[i].last_id - disjoint_id_ranges_items[i].first_id + 1;
    }

    vector_free(&disjoint_id_ranges);
    disjoint_id_ranges_items = NULL;

    vector_free(&fresh_id_ranges);
    fresh_id_ranges_items = NULL;

    assert(num_fresh == DAY5_PART1_ANS);
    assert(num_all_fresh == DAY5_PART2_ANS);

    printf("Day 5\n");
    printf("Part 1: %d\n", num_fresh);
    printf("Part 2: %ld\n", num_all_fresh);
}
