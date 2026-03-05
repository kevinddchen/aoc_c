#include "matrix.h"

#include <stdio.h>

void test_matrix_ints()
{
    printf("text_matrix_ints: ");

    Matrix m = {};
    matrix_init(&m, 3, 3, sizeof(int));

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            assert(*(int*)matrix_at_const(&m, row, col) == 0);
            *(int*)matrix_at(&m, row, col) = row * 100 + col;
        }
    }

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            assert(*(int*)matrix_at_const(&m, row, col) == row * 100 + col);
        }
    }

    matrix_free(&m);
    printf("Pass!\n");
}

void test_matrix_longs()
{
    printf("text_matrix_longs: ");

    Matrix m = {};
    matrix_init(&m, 3, 3, sizeof(long));

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            assert(*(long*)matrix_at_const(&m, row, col) == 0);
            *(long*)matrix_at(&m, row, col) = row * 1'000'000'000'000 + col;
        }
    }

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            assert(*(long*)matrix_at_const(&m, row, col) == row * 1'000'000'000'000 + col);
        }
    }

    matrix_free(&m);
    printf("Pass!\n");
}

int main()
{
    test_matrix_ints();
    test_matrix_longs();
    return EXIT_SUCCESS;
}
