#include "algo.h"
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
 * Returns if ingredient ID is fresh, i.e. falls within one of the ranges.
 */
bool is_fresh(long id, const IDRange* ranges, size_t num_ranges)
{
    for (size_t i = 0; i < num_ranges; i++) {
        if (id >= ranges[i].first_id && id <= ranges[i].last_id)
            return true;
    }
    return false;
}

/**
 * Given a list of ranges, we construct a list of disjoint ranges.
 * @param ranges Vector of `IDRange` ranges.
 * @param disjoint_ranges Output vector of `IDRange` disjoint ranges.
 */
void construct_disjoint_ranges(const Vector* ranges, Vector* disjoint_ranges)
{
    vector_init(disjoint_ranges, sizeof(IDRange));

    // the strategy is to use `disjoint_ranges` to keep track of the current list of disjoint ranges, and iterate
    // through each element of `ranges` and trying to add to `disjoint_ranges`. The only tricky thing is comparing
    // ranges if they overlap, in which case we should include their union to the list of disjoint ranges.

    for (size_t i = 0; i < ranges->count; i++) {
        // copy existing range
        IDRange range = ((IDRange*)ranges->items)[i];

        Vector new_disjoint_ranges = {};
        vector_init(&new_disjoint_ranges, sizeof(IDRange));
        vector_reserve(&new_disjoint_ranges, disjoint_ranges->count);

        // compare to existing disjoint ranges
        for (size_t j = 0; j < disjoint_ranges->count; j++) {
            const IDRange* disjoint_range = (IDRange*)disjoint_ranges->items + j;
            if (disjoint_range->last_id < range.first_id || range.last_id < disjoint_range->first_id) {
                // if ranges are disjoint, we keep the jth element
                vector_push_back(&new_disjoint_ranges, disjoint_range);
            } else {
                // otherwise, drop jth element but update `range` with larger limits
                range.first_id = MIN(disjoint_range->first_id, range.first_id);
                range.last_id = MAX(disjoint_range->last_id, range.last_id);
            }
        }
        vector_push_back(&new_disjoint_ranges, &range);

        vector_move(&new_disjoint_ranges, disjoint_ranges);
    }
}

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    Vector fresh_id_ranges = {};
    vector_init(&fresh_id_ranges, sizeof(IDRange));

    // read fresh ingredient id ranges
    char buff[1024] = {};
    while (fgets(buff, sizeof buff, fp) != NULL) {
        // break on empty line
        if (strlen(buff) == 1) {
            assert(buff[0] == '\n');
            break;
        }

        // parse two numbers separated by a dash
        char* ptr = buff;
        const long first = util_strtol(ptr, &ptr, 0);
        const long second = util_strtol(ptr + 1, NULL, 0);

        vector_push_back(&fresh_id_ranges, &(IDRange){first, second});
    }

    // === PART 1 =============================================================

    // track number of fresh ingredients
    int num_fresh = 0;

    // read available ingredient ids
    while (fgets(buff, sizeof buff, fp) != NULL) {
        const long id = util_atol(buff);
        if (is_fresh(id, fresh_id_ranges.items, fresh_id_ranges.count))
            num_fresh++;
    }

    // === PART 2 =============================================================

    Vector disjoint_id_ranges = {};
    construct_disjoint_ranges(&fresh_id_ranges, &disjoint_id_ranges);

    // tracks number of all fresh ingredients by computing lengths of the disjoint ranges
    long num_all_fresh = 0;

    for (size_t i = 0; i < disjoint_id_ranges.count; i++) {
        const IDRange* disjoint_id_range = (IDRange*)disjoint_id_ranges.items + i;
        num_all_fresh += disjoint_id_range->last_id - disjoint_id_range->first_id + 1;
    }

    // ========================================================================

    printf("Day 5\n");
    printf("Part 1: %d\n", num_fresh);
    printf("Part 2: %ld\n", num_all_fresh);

    assert(num_fresh == DAY5_PART1_ANS);
    assert(num_all_fresh == DAY5_PART2_ANS);

    vector_free(&disjoint_id_ranges);
    vector_free(&fresh_id_ranges);
    fclose(fp);
    fp = NULL;

    return EXIT_SUCCESS;
}
