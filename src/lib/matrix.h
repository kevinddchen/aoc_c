#pragma once

#include <assert.h>
#include <stdlib.h>

/**
 * Type-erased 2D matrix. Uses row-major ordering.
 */
typedef struct {
    void* data;
    size_t rows;
    size_t cols;
    size_t item_size;
} Matrix;

/**
 * Initialize a zero matrix.
 * @param m Matrix.
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @param item_size Size of matrix item, in bytes.
 */
static inline void matrix_init(Matrix* m, size_t rows, size_t cols, size_t item_size) {
    m->data = calloc(rows * cols, item_size);
    assert(m->data != NULL);
    m->rows = rows;
    m->cols = cols;
    m->item_size = item_size;
}

/**
 * Mutable pointer to element of matrix.
 * @param m Matrix.
 * @param row Row of element.
 * @param col Column of element.
 */
static inline void* matrix_at_mut(Matrix* m, size_t row, size_t col) {
    return (char*)m->data + (row * m->cols + col) * m->item_size;
}

/**
 * Pointer to element of matrix.
 * @param m Matrix.
 * @param row Row of element.
 * @param col Column of element.
 */
static inline const void* matrix_at(const Matrix* m, size_t row, size_t col) {
    return matrix_at_mut((Matrix*)m, row, col);
}

/**
 * Deallocate matrix's memory.
 * @param m Matrix.
 */
static inline void matrix_free(Matrix* m) {
    free(m->data);
    m->data = NULL;
}
