#include "io.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>

static const char FILENAME[] = "files/day6.txt";

static const long DAY6_PART1_ANS = 4693419406682;

/**
 * Parse rows containing numbers.
 * @param lines Vector of strings, representing lines of the file.
 * @param number_rows Output vector of vector of longs.
 */
void parse_number_rows(const Vector* lines, Vector* number_rows)
{
    vector_init(number_rows, sizeof(Vector));

    for (size_t i = 0; i < lines->count - 1; i++) {
        Vector row = {};
        vector_init(&row, sizeof(long));

        const char* char_ptr = ((char**)lines->items)[i];
        while (1) {
            // skip whitespace
            while (*char_ptr == ' ')
                char_ptr++;

            if (*char_ptr == '\0')
                break;

            char* next_char_ptr = {};
            const long num = strtol(char_ptr, &next_char_ptr, 0);
            assert(char_ptr != next_char_ptr);  // check conversion performed

            vector_push_back(&row, &num);
            char_ptr = next_char_ptr;
        }
        vector_push_back(number_rows, &row);
    }
}

/**
 * Parse row containing symbols.
 * @param lines Vector of strings, representing lines of the file.
 * @param symbol_row Output vector of chars.
 */
void parse_symbol_row(const Vector* lines, Vector* symbol_row)
{
    vector_init(symbol_row, sizeof(char));

    const char* last_line = ((char**)lines->items)[lines->count - 1];
    for (const char* char_ptr = last_line; *char_ptr != '\0'; char_ptr++) {
        // skip whitespace
        if (*char_ptr == ' ')
            continue;

        vector_push_back(symbol_row, char_ptr);
    }
}

int main()
{
    Vector lines = {};
    io_readlines(FILENAME, 4096, &lines);
    // for (size_t i = 0; i < lines.count; i++)
    //     printf("\"%s\"\n", ((char**)lines.items)[i]);

    // parse rows of numbers
    Vector number_rows = {};
    parse_number_rows(&lines, &number_rows);
    // for (size_t i = 0; i < number_rows.count; i++) {
    //     const Vector* number_row = (Vector*)number_rows.items + i;
    //     for (size_t j = 0; j < number_row->count; j++) {
    //         printf("%ld ", ((long*)number_row->items)[j]);
    //     }
    //     printf("\n");
    // }

    // parse row of symbols
    Vector symbol_row = {};
    parse_symbol_row(&lines, &symbol_row);
    // for (size_t i = 0; i < symbol_row.count; i++) {
    //     printf("%c ", ((char*)symbol_row.items)[i]);
    // }
    // printf("\n");

    // assert all rows same length
    const size_t n_cols = symbol_row.count;
    for (size_t i = 0; i < number_rows.count; i++)
        assert(((Vector*)number_rows.items)[i].count == n_cols);

    // === PART 1 =============================================================

    long total = 0;

    for (size_t col = 0; col < n_cols; col++) {
        long val = {};

        const char symbol = ((char*)symbol_row.items)[col];
        if (symbol == '+')
            val = 0;
        else
            val = 1;

        for (size_t row = 0; row < number_rows.count; row++) {
            const Vector* number_row = ((Vector*)number_rows.items) + row;
            const long number = ((long*)number_row->items)[col];
            if (symbol == '+')
                val += number;
            else
                val *= number;
        }

        total += val;
    }

    // === CLEANUP ============================================================

    for (size_t i = 0; i < lines.count; i++)
        free(((char**)lines.items)[i]);
    vector_free(&lines);

    for (size_t i = 0; i < number_rows.count; i++)
        vector_free((Vector*)number_rows.items + i);
    vector_free(&number_rows);

    vector_free(&symbol_row);

    assert(total == DAY6_PART1_ANS);

    printf("Day 6\n");
    printf("Part 1: %ld\n", total);
}
