#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char FILENAME[] = "files/day4.txt";

// Represents offsets between a cell and its 8 neighbors.
typedef struct {
    int dcol;
    int drow;
} Offset;

// All 8 possible offsets.
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
static const int NUM_OFFSETS = sizeof(OFFSETS) / sizeof(OFFSETS[0]);

// A roll is "accessible" if has fewer than this many neighboring rolls.
static const int ACCESSIBLE_THRESHOLD = 4;

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    // figure out dimensions of grid
    size_t cols = {};
    size_t rows = {};
    {
        cols = 0;
        while (fgetc(fp) != '\n') {
            cols++;
        }
        rows = 1;
        int c = {};
        while ((c = fgetc(fp)) != EOF) {
            if (c == '\n')
                rows++;
        }
    }

    // 2d grid is represented in memory as 1d array. we use row-major ordering.
    char* grid = malloc(cols * rows * sizeof(char));
    assert(grid != NULL);

    // copy file into grid
    {
        fseek(fp, 0, SEEK_SET);  // set file pointer to beginning of file
        for (size_t row = 0; row < rows; row++) {
            fread(grid + row * cols, sizeof(char), cols, fp);  // copy row into grid
            fseek(fp, 1, SEEK_CUR);                            // skip newline char
        }
    }
    fclose(fp);

    int forklift_accessible_rolls = 0;

    // look for all `@` in grid surrounded by fewer than 4 `@`s
    for (int row = 0; row < (int)rows; row++) {
        for (int col = 0; col < (int)cols; col++) {
            const char c = grid[col + row * cols];
            if (c != '@')
                continue;

            // iterate over all neighbors
            int num_roll_neighbors = 0;
            for (size_t i = 0; i < NUM_OFFSETS; i++) {
                const Offset* offset = &OFFSETS[i];
                const int neighbor_col = col + offset->dcol;
                const int neighbor_row = row + offset->drow;
                if (neighbor_col < 0 || neighbor_col >= (int)cols || neighbor_row < 0 || neighbor_row >= (int)rows)
                    continue;
                if (grid[neighbor_col + neighbor_row * cols] == '@')
                    num_roll_neighbors++;
            }
            if (num_roll_neighbors < ACCESSIBLE_THRESHOLD)
                forklift_accessible_rolls++;
        }
    }
    free(grid);

    printf("Day 4\n");
    printf("Part 1: %d\n", forklift_accessible_rolls);

    return EXIT_SUCCESS;
}
