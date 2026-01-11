#include "io.h"
#include "util.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>

static const char FILENAME[] = "files/day6.txt";

static const long DAY6_PART1_ANS = 4693419406682;
static const long DAY6_PART2_ANS = 9029931401920;

/**
 * Parse row containing symbols.
 * @param line String, representing last line containing symbols.
 * @param symbols Output vector of char.
 * @param block_widths Output vector of size_t; number of chars to next symbol (less one to account for ' ' delimiter).
 */
void parse_symbol_row(const char* line, Vector* symbols, Vector* block_widths)
{
    vector_init(symbols, sizeof(char));
    vector_init(block_widths, sizeof(size_t));

    size_t block_width = 0;
    for (const char* char_ptr = line; *char_ptr != '\0'; char_ptr++) {
        block_width++;

        // skip whitespace
        if (*char_ptr == ' ')
            continue;

        vector_push_back(symbols, char_ptr);

        if (block_width > 1) {  // skip first char
            block_width -= 2;   // account for ' ' delimiter and next symbol
            vector_push_back(block_widths, &block_width);
            block_width = 1;
        }
    }

    // last block
    assert(block_width > 0);
    vector_push_back(block_widths, &block_width);

    assert(symbols->count == block_widths->count);
}

/**
 * Copy a block of characters from an array of equal-length strings. This function does not check out-of-bounds memory
 * access.
 * @param lines Array of strings.
 * @param num_lines Number of lines.
 * @param offset Start copying from this character offset.
 * @param count Number of characters to copy.
 * @param block Output vector of strings. All strings need to be freed to avoid memory leak.
 */
void copy_block(const char** lines, size_t num_lines, size_t offset, size_t count, Vector* block)
{
    vector_init(block, sizeof(char*));

    for (size_t row = 0; row < num_lines; row++) {
        char* line = malloc((count + 1) * sizeof(char));
        assert(line != NULL);

        // copy characters, append null
        memcpy(line, lines[row] + offset, count * sizeof(char));
        line[count] = '\0';

        vector_push_back(block, &line);
    }
}

/**
 * Transpose a block of characters.
 * @param block Vector of strings.
 * @param block_width Length of each string of `block`, in characters.
 * @param transposed_block Output vector of strings. All strings need to be freed to avoid memory leak.
 */
void transpose_block(const Vector* block, size_t block_width, Vector* transposed_block)
{
    size_t block_height = block->count;

    // initialize `transposed_block` to strings filled with spaces
    vector_init(transposed_block, sizeof(char*));
    for (size_t t_row = 0; t_row < block_width; t_row++) {
        char* line = malloc((block_height + 1) * sizeof(char));
        assert(line != NULL);

        memset(line, ' ', block_height);
        line[block_height] = '\0';
        vector_push_back(transposed_block, &line);
    }

    // copy chars from `block`, transposed
    for (size_t row = 0; row < block_height; row++) {
        for (size_t col = 0; col < block_width; col++) {
            ((char**)transposed_block->items)[col][row] = ((char**)block->items)[row][col];
        }
    }
}

/**
 * Given a block of numbers and an operation, calculate answer assuming each row represents a number.
 * @param block Vector of strings, each row containing one number.
 * @param op Character, either '+' or '*'.
 * @returns Result of computation.
 */
long calculate_block(const Vector* block, char op)
{
    long val = op == '+' ? 0 : 1;

    for (size_t row = 0; row < block->count; row++) {
        const char* str = ((char**)block->items)[row];

        // parse int in row, ignoring leading spaces
        while (*str == ' ')
            str++;
        const long number = util_atol(str);

        if (op == '+')
            val += number;
        else
            val *= number;
    }

    return val;
}

/**
 * Deallocate memory of a vector of strings.
 * @param lines Vector of strings.
 */
void free_vector_of_strings(Vector* lines)
{
    for (size_t i = 0; i < lines->count; i++)
        free(((char**)lines->items)[i]);
    vector_free(lines);
}

int main()
{
    Vector lines = {};
    io_readlines(FILENAME, 4096, &lines);

    // check all lines have same length
    assert(lines.count > 0);
    const size_t line_length = strlen(((char**)lines.items)[0]);
    for (size_t i = 1; i < lines.count; i++)
        assert(strlen(((char**)lines.items)[i]) == line_length);

    // parse symbols
    Vector symbols = {};
    Vector block_widths = {};
    parse_symbol_row(((char**)lines.items)[lines.count - 1], &symbols, &block_widths);

    long total_p1 = 0;
    long total_p2 = 0;

    size_t block_x_offset = 0;  // x offset, in number of characters, to start of block
    for (size_t block_i = 0; block_i < symbols.count; block_i++) {
        const size_t block_width = ((size_t*)block_widths.items)[block_i];

        Vector block = {};
        copy_block(lines.items, lines.count - 1, block_x_offset, block_width, &block);

        Vector transposed_block = {};
        transpose_block(&block, block_width, &transposed_block);

        const char op = ((char*)symbols.items)[block_i];
        total_p1 += calculate_block(&block, op);
        total_p2 += calculate_block(&transposed_block, op);

        free_vector_of_strings(&transposed_block);
        free_vector_of_strings(&block);

        block_x_offset += block_width + 1;  // account for ' ' delimiter
    }

    printf("Day 6\n");
    printf("Part 1: %ld\n", total_p1);
    printf("Part 2: %ld\n", total_p2);

    assert(total_p1 == DAY6_PART1_ANS);
    assert(total_p2 == DAY6_PART2_ANS);

    vector_free(&block_widths);
    vector_free(&symbols);
    free_vector_of_strings(&lines);
}
