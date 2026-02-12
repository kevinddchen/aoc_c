#include "matrix.h"
#include "util.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const char FILENAME[] = "files/day10.txt";

static const long DAY10_PART1_ANS = 479;

static const size_t UNSET_INDEX = -1;

/**
 * Parses the indicator light to its bit-wise representation.
 * @param str Pointer to the '[' character.
 * @param str_end Output pointer to the ']' character.
 * @returns Unsigned integer representing the bits.
 */
uint32_t parse_indicator_light(char* str, char** str_end)
{
    str++;  // skip '['
    uint32_t result = 0;
    uint32_t base = 1;
    while (*str != ']') {
        if (*str == '#')
            result += base;
        base *= 2;
        str++;
    }

    if (str_end != NULL)
        *str_end = str;

    return result;
}

/**
 * Parses the button to its bit-wise representation.
 * @param str Pointer to the '(' character.
 * @param str_end Output pointer to the ')' character.
 * @returns Unsigned integer representing the bits.
 */
uint32_t parse_button(char* str, char** str_end)
{
    uint32_t result = 0;
    while (*str != ')') {
        str++;  // skip '(' or ','
        const int power = util_ctoi(*str);
        result += 1 << power;
        str++;
    }

    if (str_end != NULL)
        *str_end = str;

    return result;
}

/**
 * Parse the joltage levels.
 * @param str Pointer to the '{' character.
 * @param joltage Output vector of `int` joltage levels.
 */
void parse_joltage_levels(char* str, Vector* joltages)
{
    vector_init(joltages, sizeof(int));

    while (*str != '}') {
        str++;  // skip '{' or ','
        const int joltage = util_strtol(str, &str, 0);
        vector_push_back(joltages, &joltage);
    }
}

/**
 * Compute minimum number of button presses to achieve the given indicator light.
 * @param indicator_light Bit-wise representation of indicator light.
 * @param buttons Vector of `uint32_t` bit-wise representations of buttons.
 */
int compute_min_button_presses(uint32_t indicator_light, const Vector* buttons)
{
    if (indicator_light == 0)
        return 0;

    // TODO: use hashset for better performance
    Vector current_combinations = {};
    vector_init(&current_combinations, sizeof(uint32_t));
    vector_push_back(&current_combinations, &indicator_light);

    int num_presses = 0;

    for (size_t n = 0; n < buttons->count; n++) {
        num_presses++;

        Vector next_combinations = {};
        vector_init(&next_combinations, sizeof(uint32_t));
        vector_reserve(&next_combinations, current_combinations.count * buttons->count);

        // iterate over `current_combinations` x `buttons`
        for (size_t i = 0; i < current_combinations.count; i++) {
            for (size_t j = 0; j < buttons->count; j++) {
                const uint32_t curr = ((uint32_t*)current_combinations.items)[i];
                const uint32_t button = ((uint32_t*)buttons->items)[j];
                const uint32_t next = curr ^ button;  // take bit-wise XOR
                if (next == 0) {
                    vector_free(&current_combinations);
                    vector_free(&next_combinations);
                    return num_presses;
                }
                vector_push_back(&next_combinations, &next);
            }
        }

        // move `next_combinations` into `current_combinations`
        vector_free(&current_combinations);
        current_combinations.items = next_combinations.items;
        current_combinations.count = next_combinations.count;
        current_combinations.capacity = next_combinations.capacity;
    }

    assert(false);  // Unexpected: could not get to indicator light
}

/**
 * Take row `value_row`, multiply by `multiplier`, and add to row `target_row`.
 * @param m Matrix of `int` to be modified in-place.
 * @param target_row Row to be modified.
 * @param value_row Row to take values from; is unmodified.
 * @param multiplier Multiplied against `value_row`.
 */
void matrix_add_row(Matrix* m, size_t target_row, size_t value_row, int multiplier)
{
    int* target_row_ptr = (int*)matrix_at_mut(m, target_row, 0);
    const int* value_row_ptr = (int*)matrix_at(m, value_row, 0);

    for (size_t col = 0; col < m->cols; col++)
        target_row_ptr[col] += value_row_ptr[col] * multiplier;
}

void matrix_print(const Matrix* m)
{
    const int* array = m->data;
    printf("\n");
    for (size_t row = 0; row < m->rows; row++) {
        for (size_t col = 0; col < m->cols; col++) {
            printf("%-2d ", array[row * m->cols + col]);
        }
        printf("\n");
    }
}

/**
 * Initialize tableau of the linear program.
 * @param num_lights Number of lights, equals the number of bits in each button and number of joltage levels.
 * @param buttons Vector of `uint32_t` bit-wise representations of buttons.
 * @param joltages Vector of `int` joltage levels.
 * @param tableau Output matrix of `int`.
 */
void create_tableau(size_t num_lights, const Vector* buttons, const Vector* joltages, Matrix* tableau)
{
    const size_t rows = num_lights + 1;
    const size_t cols = buttons->count + 1;
    matrix_init(tableau, rows, cols, sizeof(int));

    // init objective function row, x1 + x2 + ... + xn = X (want to minimize)
    for (size_t col = 0; col < cols - 1; col++)
        *(int*)matrix_at_mut(tableau, 0, col) = 1;

    // init constraint rows, x1 v1 + x2 v2 + ... + xn vn = b, where v1, v2, ..., vn are the button vectors
    for (size_t col = 0; col < cols - 1; col++) {
        uint32_t button = ((uint32_t*)buttons->items)[col];
        // read bits off `button` one-by-one
        for (size_t row = 1; row < rows; row++) {
            const int bit = button & 1;
            *(int*)matrix_at_mut(tableau, row, col) = bit;
            button >>= 1;
        }
        assert(button == 0);  // all bits should be used
    }

    // init b vector from joltage levels
    for (size_t row = 1; row < rows; row++)
        *(int*)matrix_at_mut(tableau, row, cols - 1) = ((int*)joltages->items)[row - 1];
}

/**
 * Return a pivot column that will reduce the objective function. Otherwise, returns `UNSET_INDEX`.
 *
 * This is implemented by picking any negative value in the objective function row.
 *
 * @param tableau Matrix of `int`.
 * @param auxiliary True if this is the auxiliary tableau.
 */
size_t find_pivot_column(const Matrix* tableau, bool auxiliary)
{
    // if auxiliary, do not pick pivot from auxiliary variables
    const size_t start_col = auxiliary ? tableau->rows - 2 : 0;

    for (size_t col = start_col; col < tableau->cols - 1; col++) {
        if (*(int*)matrix_at(tableau, 0, col) < 0)
            return col;
    }
    return UNSET_INDEX;
}

/**
 * Given pivot column, return a pivot row that ensures all x1, x2, ..., xn variables remain nonnegative.
 *
 * This is implemented by picking the row with minimum b_r / a_{rc} ratio.
 *
 * @param tableau Matrix of `int`.
 * @param pivot_col Pivot column.
 * @param auxiliary True if this is the auxiliary tableau.
 */
size_t find_pivot_row(const Matrix* tableau, size_t pivot_col, bool auxiliary)
{
    // if auxiliary, first two rows are objective rows. otherwise, first row is objective row.
    const size_t start_row = auxiliary ? 2 : 1;

    size_t pivot_row = UNSET_INDEX;
    double min_ratio = {};
    for (size_t row = start_row; row < tableau->rows; row++) {
        const int a = *(int*)matrix_at(tableau, row, pivot_col);
        const int b = *(int*)matrix_at(tableau, row, tableau->cols - 1);

        if (a <= 0)
            continue;
        assert(b >= 0);

        const double ratio = b / (double)a;
        if (pivot_row == UNSET_INDEX || ratio <= min_ratio) {
            min_ratio = ratio;
            if (a > 1)
                continue;  // HACK: force pivot element to be 1
            pivot_row = row;
        }
    }
    assert(pivot_row != UNSET_INDEX);
    return pivot_row;
}

/**
 * Perform pivot operation on the given column.
 * @param tableau Matrix of `int` to be modified in-place.
 * @param pivot_row Pivot row.
 * @param pivot_col Pivot column.
 */
void pivot(Matrix* tableau, size_t pivot_row, size_t pivot_col)
{
    const int pivot_el = *(int*)matrix_at(tableau, pivot_row, pivot_col);
    assert(pivot_el == 1);  // HACK: is this always true?

    for (size_t row = 0; row < tableau->rows; row++) {
        // skip pivot row
        if (row == pivot_row)
            continue;

        // reduce row by adding multiple of pivot row
        const int el = *(int*)matrix_at(tableau, row, pivot_col);
        if (el == 0)
            continue;

        matrix_add_row(tableau, row, pivot_row, -el);
    }
}

/**
 * Repeatedly perform pivot operations.
 * @param tableau Matrix of `int` to be modified in-place.
 * @param auxiliary True if this is the auxiliary tableau.
 */
void loop_pivot(Matrix* tableau, bool auxiliary)
{
    size_t pivot_col = {};
    while (pivot_col = find_pivot_column(tableau, auxiliary), pivot_col != UNSET_INDEX) {
        const size_t pivot_row = find_pivot_row(tableau, pivot_col, auxiliary);
        printf("pivot: r=%zu c=%zu\n", pivot_row, pivot_col);
        pivot(tableau, pivot_row, pivot_col);
        matrix_print(tableau);
    }
}

/**
 * Create auxiliary tableau, which is used to put `tableau` into canonical form.
 * @param tableau Matrix of `int`.
 * @param aux_tableau Output matrix of `int`.
 */
void create_auxiliary_tableau(const Matrix* tableau, Matrix* aux_tableau)
{
    const size_t num_aux_vars = tableau->rows - 1;

    const size_t rows = tableau->rows + 1;
    const size_t cols = tableau->cols + num_aux_vars;
    matrix_init(aux_tableau, rows, cols, sizeof(int));

    // copy tableau
    for (size_t row = 0; row < tableau->rows; row++) {
        for (size_t col = 0; col < tableau->cols; col++)
            *(int*)matrix_at_mut(aux_tableau, row + 1, col + num_aux_vars) = *(int*)matrix_at(tableau, row, col);
    }

    // init auxiliary objective function row: y1 + y2 + ... + ym = Y (want to minimize to 0)
    for (size_t col = 0; col < num_aux_vars; col++)
        *(int*)matrix_at_mut(aux_tableau, 0, col) = 1;

    // init constraints for auxiliary variables, which is diagonal matrix
    for (size_t i = 0; i < num_aux_vars; i++)
        *(int*)matrix_at_mut(aux_tableau, i + 2, i) = 1;

    // subtract rows from auxiliary objecive function
    for (size_t row = 2; row < rows; row++)
        matrix_add_row(aux_tableau, 0, row, -1);
}

/**
 * Copy original tableau back into `tableau` from `aux_tableau`; effectively an inverse of the
 * `create_auxiliary_tableau()` function.
 * @param aux_tableau Matrix of `int`.
 * @param tableau Output matrix of `int`.
 */
void extract_original_tableau(const Matrix* aux_tableau, Matrix* tableau)
{
    const size_t num_aux_vars = aux_tableau->rows - 2;

    const size_t rows = aux_tableau->rows - 1;
    const size_t cols = aux_tableau->cols - num_aux_vars;
    matrix_init(tableau, rows, cols, sizeof(int));

    for (size_t row = 0; row < tableau->rows; row++) {
        for (size_t col = 0; col < tableau->cols; col++)
            *(int*)matrix_at_mut(tableau, row, col) = *(int*)matrix_at(aux_tableau, row + 1, col + num_aux_vars);
    }
}

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    int total_button_presses_p1 = 0;
    int total_button_presses_p2 = 0;

    // iterate over each line
    char buff[1024] = {};
    while (fgets(buff, sizeof buff, fp) != NULL) {
        char* ptr = buff;

        // parse data from the line

        assert(*ptr == '[');
        const uint32_t indicator_light = parse_indicator_light(ptr, &ptr);
        const size_t num_lights = ptr - buff - 1;

        Vector buttons = {};
        vector_init(&buttons, sizeof(uint32_t));

        ptr += 2;  // skip '] '
        assert(*ptr == '(');
        while (*ptr == '(') {
            const uint32_t button = parse_button(ptr, &ptr);
            vector_push_back(&buttons, &button);
            ptr += 2;  // skip ') '
        }

        assert(*ptr == '{');
        Vector joltages = {};
        parse_joltage_levels(ptr, &joltages);
        assert(joltages.count == num_lights);

        // === PART 1 =========================================================

        total_button_presses_p1 += compute_min_button_presses(indicator_light, &buttons);

        // === PART 2 =========================================================

        // solve linear problem using simplex algorithm
        // https://en.wikipedia.org/wiki/Simplex_algorithm

        Matrix tableau = {};
        create_tableau(num_lights, &buttons, &joltages, &tableau);
        matrix_print(&tableau);

        // to put `tableau` into canonical form, we first solve the auxiliary problem

        Matrix aux_tableau = {};
        create_auxiliary_tableau(&tableau, &aux_tableau);
        matrix_free(&tableau);
        matrix_print(&aux_tableau);

        loop_pivot(&aux_tableau, true);

        assert(*(int*)matrix_at(&aux_tableau, 0, aux_tableau.cols - 1) == 0);  // check solved auxiliary problem
        extract_original_tableau(&aux_tableau, &tableau);
        matrix_print(&tableau);

        // now that the original problem is in canonical form, it can be solved

        loop_pivot(&tableau, true);

        const int joltage_presses = -*(int*)matrix_at(&tableau, 0, tableau.cols - 1);
        printf("%d\n", joltage_presses);
        total_button_presses_p2 += joltage_presses;

        matrix_free(&tableau);
        matrix_free(&aux_tableau);

        // ====================================================================

        vector_free(&joltages);
        vector_free(&buttons);
    }

    printf("Day 10\n");
    printf("Part 1: %d\n", total_button_presses_p1);
    printf("Part 2: %d\n", total_button_presses_p2);

    assert(total_button_presses_p1 == DAY10_PART1_ANS);

    fclose(fp);
    fp = NULL;

    return EXIT_SUCCESS;
}
