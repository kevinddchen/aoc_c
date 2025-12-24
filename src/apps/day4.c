#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char FILENAME[] = "files/day4.txt";

static const int DAY4_PART1_ANS = 1320;
static const int DAY4_PART2_ANS = 8354;

// Represents offset vectors between a cell and its 8 neighbors.
typedef struct {
    int dcol;
    int drow;
} Offset;

// All 8 possible offset vectors to a cell's neighbors.
static const Offset OFFSETS[] = {
    {-1, -1},
    {0, -1},
    {1, -1},
    {-1, 0},
    {1, 0},
    {-1, 1},
    {0, 1},
    {1, 1},
};
static const size_t NUM_OFFSETS = sizeof(OFFSETS) / sizeof(OFFSETS[0]);

// A roll is "accessible" if has fewer than this many neighboring rolls.
static const int ACCESSIBLE_THRESHOLD = 4;

/**
 * Read the file and obtain grid.
 * @param grid_ptr Output grid.
 * @param cols_pts Output mumber of columns.
 * @param rows_ptr Output number of rows.
 */
void initialize_grid(char** restrict grid_ptr, size_t* restrict cols_ptr, size_t* restrict rows_ptr)
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    // figure out dimensions of grid
    size_t cols = 0;
    while (fgetc(fp) != '\n') {
        cols++;
    }
    size_t rows = 1;
    int c = {};
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n')
            rows++;
    }

    // 2d grid is represented in memory as 1d array. we use row-major ordering.
    char* grid = malloc(cols * rows * sizeof(char));
    assert(grid != NULL);

    // copy file into grid
    fseek(fp, 0, SEEK_SET);  // set file pointer to beginning of file
    for (size_t row = 0; row < rows; row++) {
        fread(grid + row * cols, sizeof(char), cols, fp);  // copy row into grid
        fseek(fp, 1, SEEK_CUR);                            // skip newline char
    }

    fclose(fp);
    *grid_ptr = grid;
    *cols_ptr = cols;
    *rows_ptr = rows;
}

/**
 * Removes accessible rolls from the grid, replacing `@` with `x` character.
 * @param grid_ptr Grid; will be modified by this function.
 * @param cols Number of columns.
 * @param rows Number of rows.
 * @returns Number of accessible rolls that were removed rolls.
 */
int remove_accessible_rolls(char** restrict grid_ptr, size_t cols, size_t rows)
{
    const size_t grid_size_bytes = cols * rows * sizeof(char);

    // make copy of grid
    char* grid_copy = malloc(grid_size_bytes);
    assert(grid_copy != NULL);
    memcpy(grid_copy, *grid_ptr, grid_size_bytes);

    int removed_rolls = 0;

    // look for all `@` in grid surrounded by fewer than 4 `@`s
    for (int row = 0; row < (int)rows; row++) {
        for (int col = 0; col < (int)cols; col++) {
            const char c = grid_copy[col + row * cols];
            if (c != '@')
                continue;

            // iterate over all neighbors, count number of rolls
            int num_roll_neighbors = 0;
            for (size_t i = 0; i < NUM_OFFSETS; i++) {
                const Offset* offset = &OFFSETS[i];
                const int neighbor_col = col + offset->dcol;
                const int neighbor_row = row + offset->drow;
                if (neighbor_col < 0 || neighbor_col >= (int)cols || neighbor_row < 0 || neighbor_row >= (int)rows)
                    continue;
                if (grid_copy[neighbor_col + neighbor_row * cols] == '@')
                    num_roll_neighbors++;
            }
            if (num_roll_neighbors < ACCESSIBLE_THRESHOLD) {
                removed_rolls++;
                (*grid_ptr)[col + row * cols] = 'x';  // mark roll as removed
            }
        }
    }
    return removed_rolls;
}

int main()
{
    char* grid = {};
    size_t cols = {};
    size_t rows = {};
    initialize_grid(&grid, &cols, &rows);

    const int accessible_rolls = remove_accessible_rolls(&grid, cols, rows);
    int total_removed_rolls = accessible_rolls;
    int removed_rolls = {};
    while ((removed_rolls = remove_accessible_rolls(&grid, cols, rows)) > 0)
        total_removed_rolls += removed_rolls;

    free(grid);

    assert(accessible_rolls == DAY4_PART1_ANS);
    assert(total_removed_rolls == DAY4_PART2_ANS);

    printf("Day 4\n");
    printf("Part 1: %d\n", accessible_rolls);
    printf("Part 2: %d\n", total_removed_rolls);

    return EXIT_SUCCESS;
}
