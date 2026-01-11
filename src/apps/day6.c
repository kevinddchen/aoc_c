#include "io.h"
#include "util.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>

static const char FILENAME[] = "files/day6.txt";

static const long DAY6_PART1_ANS = 4693419406682;

/**
 * Parse row containing symbols.
 * @param line String, representing last line containing symbols.
 * @param symbol_row Output vector of chars.
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

int main()
{
    Vector lines = {};
    io_readlines(FILENAME, 4096, &lines);
    // for (size_t i = 0; i < lines.count; i++)
    //     printf("\"%s\"\n", ((char**)lines.items)[i]);

    // TODO: assert all lines same length

    // parse row of symbols
    Vector symbols = {};
    Vector block_widths = {};
    parse_symbol_row(((char**)lines.items)[lines.count - 1], &symbols, &block_widths);

    // === PART 1 =============================================================

    long total = 0;

    size_t block_offset = 0;  // offset, in number of characters, to start of block in the row
    for (size_t block_i = 0; block_i < symbols.count; block_i++) {
        long val = {};

        const char symbol = ((char*)symbols.items)[block_i];
        if (symbol == '+')
            val = 0;
        else
            val = 1;

        for (size_t row = 0; row < lines.count - 1; row++) {
            const char* block_str = ((char**)lines.items)[row] + block_offset;

            // parse int in row, ignoring leading spaces
            while (*block_str == ' ')
                block_str++;
            const long number = util_atol(block_str);

            if (symbol == '+')
                val += number;
            else
                val *= number;
        }
        total += val;

        block_offset += ((size_t*)block_widths.items)[block_i] + 1;
    }

    // ========================================================================

    printf("Day 6\n");
    printf("Part 1: %ld\n", total);

    assert(total == DAY6_PART1_ANS);

    vector_free(&symbols);
    vector_free(&block_widths);
    for (size_t i = 0; i < lines.count; i++)
        free(((char**)lines.items)[i]);
    vector_free(&lines);
}
