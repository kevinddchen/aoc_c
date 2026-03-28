#include "algo.h"
#include "matrix.h"
#include "util.h"
#include "vector.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char FILENAME[] = "files/day10.txt";

static const long DAY10_PART1_ANS = 479;
static const long DAY10_PART2_ANS = 19574;

static const size_t UNSET_INDEX = -1;

/**
 * Parses the bits represented in the indicator light.
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
 * Parses the bits represented by the button.
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

    // we iterate over all combinations of buttons, and check that their bit-wise XOR product equals the indicator light
    // NOTE: we could use hashset instead to track unique elements. probably faster?
    Vector current_combinations = {};
    vector_init(&current_combinations, sizeof(uint32_t));

    // tracks current combinations of buttons. initialize with indicator light (i.e. no buttons)
    vector_push_back(&current_combinations, &indicator_light);

    for (size_t num_presses = 1; num_presses <= buttons->count; num_presses++) {
        // tracks next combinations of buttons
        Vector next_combinations = {};
        vector_init(&next_combinations, sizeof(uint32_t));
        vector_reserve(&next_combinations, current_combinations.count * buttons->count);

        // iterate over `current_combinations` x `buttons`
        const uint32_t* current_combinations_items = current_combinations.items;
        const uint32_t* buttons_items = buttons->items;
        for (size_t i = 0; i < current_combinations.count; i++) {
            for (size_t j = 0; j < buttons->count; j++) {
                const uint32_t next = current_combinations_items[i] ^ buttons_items[j];  // take XOR
                if (next == 0) {
                    vector_free(&current_combinations);
                    vector_free(&next_combinations);
                    return num_presses;
                }
                vector_push_back(&next_combinations, &next);
            }
        }

        vector_free(&current_combinations);
        vector_move(&next_combinations, &current_combinations);
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
    if (multiplier == 0)
        return;

    int* target_row_ptr = matrix_at(m, target_row, 0);
    const int* value_row_ptr = matrix_at_const(m, value_row, 0);

    for (size_t col = 0; col < m->cols; col++)
        target_row_ptr[col] += value_row_ptr[col] * multiplier;
}

/**
 * Multiply row by `multiplier`.
 * @param m Matrix of `int` to be modified in-place.
 * @param row Row to be modified.
 * @param multiplier Multiplied against `row`.
 */
void matrix_mul_row(Matrix* m, size_t row, int multiplier)
{
    if (multiplier == 1)
        return;

    int* row_ptr = matrix_at(m, row, 0);

    for (size_t col = 0; col < m->cols; col++)
        row_ptr[col] *= multiplier;
}

/**
 * Normalize row by dividing by the greatest common divisor.
 * @param m Matrix of `int` to be modified in-place.
 * @param row Row to be normalized.
 */
void matrix_norm_row(Matrix* m, size_t row)
{
    int* row_ptr = matrix_at(m, row, 0);

    int divisor = 0;
    for (size_t col = 0; col < m->cols; col++) {
        divisor = gcd(row_ptr[col], divisor);
        if (divisor == 1)
            return;
    }

    if (divisor == 0)  // entire row is 0
        return;

    for (size_t col = 0; col < m->cols; col++)
        row_ptr[col] /= divisor;
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
 *
 *  1  -1 -1 -1 -1 -1 -1 0  <-- objective function row
 *  0  0  0  0  0  1  1  3  <-- constraint rows
 *  0  0  1  0  0  0  1  5  <
 *  0  0  0  1  1  1  0  4  <
 *  0  1  1  0  1  0  0  7  <
 *     ^  ^           ^  ^
 *     v1 v2 ...      vn b
 *
 * @param buttons Vector of `uint32_t` bit-wise representations of buttons.
 * @param joltages Vector of `int` joltage levels.
 * @param tableau Output matrix of `int`.
 */
void create_tableau(const Vector* buttons, const Vector* joltages, Matrix* tableau)
{
    const size_t rows = joltages->count + 1;
    const size_t cols = buttons->count + 2;
    matrix_init(tableau, rows, cols, sizeof(int));

    *(int*)matrix_at(tableau, 0, 0) = 1;

    // init objective function row, Z - x1 - x2 - ... - xn = 0 (want to minimize Z)
    for (size_t i = 0; i < buttons->count; i++)
        *(int*)matrix_at(tableau, 0, i + 1) = -1;

    // init constraint rows, x1 v1 + x2 v2 + ... + xn vn = b, where v1, v2, ..., vn are the button vectors
    const uint32_t* buttons_items = buttons->items;
    for (size_t i = 0; i < buttons->count; i++) {
        uint32_t button = buttons_items[i];
        // read bits off `button` one-by-one
        for (size_t j = 0; j < joltages->count; j++) {
            const int bit = button & 1;
            *(int*)matrix_at(tableau, j + 1, i + 1) = bit;
            button >>= 1;
        }
        assert(button == 0);  // all bits should be used
    }

    // init b vector from joltage levels
    const int* joltages_items = joltages->items;
    for (size_t j = 0; j < joltages->count; j++)
        *(int*)matrix_at(tableau, j + 1, cols - 1) = joltages_items[j];
}

/**
 * Create auxiliary tableau, which is used to put `tableau` into canonical form.
 *
 *  1  0  0  0  0  0  0  0  -1 -1 -1 -1 0  <-- auxiliary objective function row
 *  0  1  -1 -1 -1 -1 -1 -1 0  0  0  0  0
 *  0  0  0  0  0  0  1  1  1  0  0  0  3
 *  0  0  0  1  0  0  0  1  0  1  0  0  5
 *  0  0  0  0  1  1  1  0  0  0  1  0  4
 *  0  0  1  1  0  1  0  0  0  0  0  1  7
 *                          ^  ^  ^  ^
 *                          auxiliary variables
 *
 * @param tableau Matrix of `int`.
 * @param aux_tableau Output matrix of `int`.
 */
void create_auxiliary_tableau(const Matrix* tableau, Matrix* aux_tableau)
{
    const size_t num_aux_vars = tableau->rows - 1;

    const size_t rows = tableau->rows + 1;
    const size_t cols = tableau->cols + num_aux_vars + 1;
    matrix_init(aux_tableau, rows, cols, sizeof(int));

    *(int*)matrix_at(aux_tableau, 0, 0) = 1;

    // copy tableau
    for (size_t i = 0; i < tableau->rows; i++) {
        for (size_t j = 0; j < tableau->cols - 1; j++)
            *(int*)matrix_at(aux_tableau, i + 1, j + 1) = *(int*)matrix_at_const(tableau, i, j);
        *(int*)matrix_at(aux_tableau, i + 1, cols - 1) = *(int*)matrix_at_const(tableau, i, tableau->cols - 1);
    }

    // init auxiliary objective function row: Z' - y1 - y2 - ... - ym = 0 (want to minimize Z' to 0)
    for (size_t i = 0; i < num_aux_vars; i++)
        *(int*)matrix_at(aux_tableau, 0, i + tableau->cols) = -1;

    // init constraints for auxiliary variables, which is diagonal matrix
    for (size_t i = 0; i < num_aux_vars; i++)
        *(int*)matrix_at(aux_tableau, i + 2, i + tableau->cols) = 1;

    // add all rows to auxiliary objective function
    for (size_t i = 0; i < num_aux_vars; i++)
        matrix_add_row(aux_tableau, 0, i + 2, 1);
    matrix_norm_row(aux_tableau, 0);
}

/**
 * Copy original tableau from `aux_tableau` back into `tableau`.
 * @param aux_tableau Matrix of `int`.
 * @param tableau Output matrix of `int`. Will be overwritten.
 */
void extract_original_tableau(const Matrix* aux_tableau, Matrix* tableau)
{
    for (size_t i = 0; i < tableau->rows; i++) {
        for (size_t j = 0; j < tableau->cols - 1; j++)
            *(int*)matrix_at(tableau, i, j) = *(int*)matrix_at_const(aux_tableau, i + 1, j + 1);
        *(int*)matrix_at(tableau, i, tableau->cols - 1) =
            *(int*)matrix_at_const(aux_tableau, i + 1, aux_tableau->cols - 1);

        matrix_norm_row(tableau, i);
    }
}

/**
 * Return a pivot column that will reduce the objective function. Otherwise, returns `UNSET_INDEX`.
 * @param tableau Matrix of `int`.
 * @param auxiliary True if this is the auxiliary tableau.
 */
size_t find_pivot_column(const Matrix* tableau, bool auxiliary)
{
    // if auxiliary, first two columns are for normalization. otherwise, first column is for normalization.
    const size_t start_col = auxiliary ? 2 : 1;
    // if auxiliary, do not pick pivot from auxiliary variables
    const size_t end_col = auxiliary ? tableau->cols - (tableau->rows - 2) - 2 : tableau->cols - 2;
    // if auxiliary, first two rows are objective rows. otherwise, first row is objective row.
    const size_t start_row = auxiliary ? 2 : 1;
    const size_t end_row = tableau->rows - 1;

    // Devex algorithm: pick column with greatest "steepness"
    size_t pivot_col = UNSET_INDEX;
    double max_steepness = {};

    for (size_t col = start_col; col <= end_col; col++) {
        const int rise = *(int*)matrix_at_const(tableau, 0, col);
        // "rise" must be positive to reduce objective function
        if (rise <= 0)
            continue;

        // compute "tread" of the column
        int tread_squared = 0;
        for (size_t row = start_row; row <= end_row; row++) {
            const int el = *(int*)matrix_at_const(tableau, row, col);
            tread_squared += el * el;
        }

        // "steepness" is rise over tread
        const double steepness = rise / sqrt(tread_squared);
        if (pivot_col == UNSET_INDEX || steepness > max_steepness) {
            pivot_col = col;
            max_steepness = steepness;
        }
    }
    return pivot_col;
}

/**
 * Given pivot column, return a pivot row that ensures all x1, x2, ..., xn variables remain nonnegative after pivot.
 * @param tableau Matrix of `int`.
 * @param pivot_col Pivot column.
 * @param auxiliary True if this is the auxiliary tableau.
 */
size_t find_pivot_row(const Matrix* tableau, size_t pivot_col, bool auxiliary)
{
    // if auxiliary, first two rows are objective rows. otherwise, first row is objective row.
    const size_t start_row = auxiliary ? 2 : 1;
    const size_t end_row = tableau->rows - 1;

    // pick the row with minimum b_r / a_{rc} ratio.
    size_t pivot_row = UNSET_INDEX;
    double min_ratio = {};

    for (size_t row = start_row; row <= end_row; row++) {
        const int a = *(int*)matrix_at_const(tableau, row, pivot_col);
        const int b = *(int*)matrix_at_const(tableau, row, tableau->cols - 1);

        // pivot element must be positive
        if (a <= 0)
            continue;
        assert(b >= 0);

        const double ratio = b / (double)a;
        if (pivot_row == UNSET_INDEX || ratio < min_ratio) {
            pivot_row = row;
            min_ratio = ratio;
        }
    }
    assert(pivot_row != UNSET_INDEX);
    return pivot_row;
}

/**
 * Perform pivot operation.
 * @param tableau Matrix of `int` to be modified in-place.
 * @param pivot_row Pivot row.
 * @param pivot_col Pivot column.
 */
void pivot(Matrix* tableau, size_t pivot_row, size_t pivot_col)
{
    const int pivot_el = *(int*)matrix_at_const(tableau, pivot_row, pivot_col);

    for (size_t row = 0; row < tableau->rows; row++) {
        // skip pivot row
        if (row == pivot_row)
            continue;

        const int el = *(int*)matrix_at_const(tableau, row, pivot_col);
        if (el == 0)
            continue;

        // multiply row to multiple of `pivot_el`
        matrix_mul_row(tableau, row, pivot_el);

        // reduce row by subtracting multiple of pivot row
        matrix_add_row(tableau, row, pivot_row, -el);

        // normalize row
        matrix_norm_row(tableau, row);
    }
}

/**
 * Repeatedly perform pivot operations until objective function cannot be minimized further.
 * @param tableau Matrix of `int` to be modified in-place.
 * @param auxiliary True if this is the auxiliary tableau.
 */
void loop_pivot(Matrix* tableau, bool auxiliary)
{
    size_t pivot_col = {};
    while (pivot_col = find_pivot_column(tableau, auxiliary), pivot_col != UNSET_INDEX) {
        const size_t pivot_row = find_pivot_row(tableau, pivot_col, auxiliary);
        pivot(tableau, pivot_row, pivot_col);
    }
}

/**
 * Checks that all `values[i]` are multiples of `coeffs[i]`.
 */
bool all_multiples_of_coeffs(const int* values, const int* coeffs, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (coeffs[i] == 0) {
            if (values[i] != 0)
                return false;
        } else if (values[i] % coeffs[i] != 0) {
            return false;
        }
    }
    return true;
}

/**
 * Given an array, checks that all xi >= 0, i.e. `values[i] >= 0` for i = 1, 2, ....
 */
bool all_xi_nonnegative(const int* values, size_t n)
{
    for (size_t i = 1; i < n; i++) {
        if (values[i] < 0)
            return false;
    }
    return true;
}

/**
 * Return the minimal integral score for the solved linear program.
 * @param tableau Matrix of `int`.
 */
int min_integral_score(const Matrix* tableau)
{
    // At this point, we have a reduced system of equations for variables Z, x1, x2, ..., xn and the last column being
    // the equation right-hand-side values.

    // We define "free variables" as xi left undetermined by the system of equations. These necessarily correspond to
    // columns that are not fully reduced and have more than 1 non-zero element. For each "non-free variable", we note
    // the coefficient multiplying it.

    Vector free_col_idxs = {};  // i.e. columns corresponding to "free variables"
    vector_init(&free_col_idxs, sizeof(size_t));

    int* nonfree_coeffs = calloc(tableau->rows, sizeof(int));  // coefficients in columns for "non-free variables"

    for (size_t col = 0; col <= tableau->cols - 2; col++) {
        size_t nonzero_count = 0;
        size_t nonfree_row = UNSET_INDEX;
        for (size_t row = 0; row < tableau->rows; row++) {
            if (*(int*)matrix_at_const(tableau, row, col) != 0) {
                nonzero_count++;
                nonfree_row = row;
            }
        }
        if (nonzero_count > 1) {
            vector_push_back(&free_col_idxs, &col);
        } else {
            assert(nonfree_row != UNSET_INDEX);
            nonfree_coeffs[nonfree_row] = *(int*)matrix_at_const(tableau, nonfree_row, col);
        }
    }

    assert(nonfree_coeffs[0] != 0);  // coeff corresponding to Z must be non-zero

    // For an integral solution, the RHS values must be multiples of the coefficients in `nonfree_coeffs`. We have the
    // freedom of subtracting multiples of the columns `free_cols` corresponding to the "free variables". We do a very
    // inefficient brute-force search, in the sense that we may traverse the same node of the graph multiple times. But
    // at least this is easier to implement.

    // copy free-variable columns to `free_cols`
    Vector free_cols = {};
    vector_init(&free_cols, sizeof(int*));
    vector_reserve(&free_cols, free_col_idxs.count);

    const size_t* col_idxs_items = free_col_idxs.items;
    for (size_t i = 0; i < free_col_idxs.count; i++) {
        int* free_col = malloc(tableau->rows * sizeof(int));
        for (size_t row = 0; row < tableau->rows; row++)
            free_col[row] = *(int*)matrix_at_const(tableau, row, col_idxs_items[i]);

        vector_push_back(&free_cols, &free_col);
    }

    // tracks our search. initialize with column of RHS values
    Vector value_cols = {};
    vector_init(&value_cols, sizeof(int*));

    int* value_col = malloc(tableau->rows * sizeof(int));
    for (size_t row = 0; row < tableau->rows; row++)
        value_col[row] = *(int*)matrix_at_const(tableau, row, tableau->cols - 1);

    vector_push_back(&value_cols, &value_col);

    // Brute-force search!

    int best_z = {};  // best objective function value
    bool best_z_set = false;
    for (size_t i = 0; i < value_cols.count; i++) {
        // NOTE: lots of casting, since pointer `value_cols.items` may change value
        const int* value_col = ((int**)value_cols.items)[i];

        // exit if no chance of strictly better Z
        if (best_z_set && best_z * nonfree_coeffs[0] <= value_col[0])
            continue;

        // exit if any xi are negative
        if (!all_xi_nonnegative(value_col, tableau->rows)) {
            continue;
        }

        if (all_multiples_of_coeffs(value_col, nonfree_coeffs, tableau->rows)) {
            const int z = value_col[0] / nonfree_coeffs[0];
            if (!best_z_set || z < best_z) {
                best_z = z;
                best_z_set = true;
            }
        }

        // we subtract each col in `free_cols` from `value_col`, and append to the vector
        const int** cols_items = free_cols.items;
        for (size_t j = 0; j < free_cols.count; j++) {
            int* value_col_copy = malloc(tableau->rows * sizeof(int));
            memcpy(value_col_copy, value_col, tableau->rows * sizeof(int));

            for (size_t row = 0; row < tableau->rows; row++)
                value_col_copy[row] -= cols_items[j][row];

            vector_push_back(&value_cols, &value_col_copy);
        }
    }
    assert(best_z_set);

    vector_free_arrays(&value_cols);
    vector_free_arrays(&free_cols);
    free(nonfree_coeffs);
    vector_free(&free_col_idxs);

    return best_z;
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

        // === PART 1 =========================================================

        total_button_presses_p1 += compute_min_button_presses(indicator_light, &buttons);

        // === PART 2 =========================================================

        // solve linear problem using simplex algorithm
        // https://en.wikipedia.org/wiki/Simplex_algorithm

        Matrix tableau = {};
        create_tableau(&buttons, &joltages, &tableau);

        // to put `tableau` into canonical form, we first solve the auxiliary problem

        Matrix aux_tableau = {};
        create_auxiliary_tableau(&tableau, &aux_tableau);

        loop_pivot(&aux_tableau, true);
        assert(
            *(int*)matrix_at_const(&aux_tableau, 0, aux_tableau.cols - 1) == 0);  // check auxiliary problem is solved

        extract_original_tableau(&aux_tableau, &tableau);

        // now that the original problem is in canonical form, it can be solved

        loop_pivot(&tableau, false);

        total_button_presses_p2 += min_integral_score(&tableau);

        // ====================================================================

        matrix_free(&aux_tableau);
        matrix_free(&tableau);

        vector_free(&joltages);
        vector_free(&buttons);
    }

    printf("Day 10\n");
    printf("Part 1: %d\n", total_button_presses_p1);
    printf("Part 2: %d\n", total_button_presses_p2);

    assert(total_button_presses_p1 == DAY10_PART1_ANS);
    assert(total_button_presses_p2 == DAY10_PART2_ANS);

    fclose(fp);
    fp = NULL;

    return EXIT_SUCCESS;
}
