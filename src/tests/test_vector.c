#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void test_vector_ints()
{
    printf("test_vector_ints: ");

    // build vector of squares 0, 1, 4, 9, ...
    Vector v = {};
    vector_init(&v, sizeof(int));

    const int N = 100;
    for (int i = 0; i < N; i++) {
        assert((int)v.count == i);
        int* square = vector_emplace_back(&v);
        *square = i * i;
    }

    // check vector is as expected
    assert(v.count == N);
    const int* v_items = v.items;
    for (int i = 0; i < N; i++)
        assert(v_items[i] == i * i);

    vector_free(&v);
    printf("Pass!\n");
}

typedef struct {
    int number;
    int square;
    int cube;
} Triple;

void test_vector_structs()
{
    printf("test_vector_structs: ");

    // build vector of structs
    Vector v = {};
    vector_init(&v, sizeof(Triple));

    const int N = 100;
    for (int i = 0; i < N; i++) {
        assert((int)v.count == i);
        Triple* triple = vector_emplace_back(&v);
        triple->number = i;
        triple->square = i * i;
        triple->cube = i * i * i;
    }

    // check vector is as expected
    assert(v.count == N);
    const Triple* v_items = v.items;
    for (int i = 0; i < N; i++) {
        assert(v_items[i].number == i);
        assert(v_items[i].square == i * i);
        assert(v_items[i].cube == i * i * i);
    }

    vector_free(&v);
    printf("Pass!\n");
}

void test_vector_arrays()
{
    printf("test_vector_arrays: ");

    // build vector of arrays
    Vector v = {};
    vector_init(&v, sizeof(int*));

    const int N = 10;
    for (int i = 0; i < N; i++) {
        assert((int)v.count == i);
        const int* arr = calloc(5, sizeof(int));
        vector_push_back(&v, &arr);
    }

    // check vector is as expected
    assert((int)v.count == N);

    vector_free_arrays(&v);
    printf("Pass!\n");
}

void test_vector_reserve()
{
    printf("test_vector_reserve: ");

    Vector v = {};
    vector_init(&v, sizeof(int));

    // reserve, then check that capacity does not change
    const int N = 100;
    vector_reserve(&v, N);

    assert(v.capacity == N);
    for (int i = 0; i < N; i++) {
        assert((int)v.count == i);
        vector_push_back(&v, &i);
        assert(v.capacity == N);
    }
    assert(v.count == N);

    // calling with smaller capacity does nothing
    vector_reserve(&v, 0);
    assert(v.count == N);
    assert(v.capacity == N);

    vector_free(&v);
    printf("Pass!\n");
}

void test_vector_pop_back()
{
    printf("test_vector_pop_back: ");

    // build vector
    Vector v = {};
    vector_init(&v, sizeof(int));

    const int N = 10;
    for (int i = 0; i < N; i++)
        vector_push_back(&v, &i);

    // check pop
    for (int i = N; i > 0; i--) {
        assert((int)v.count == i);
        vector_pop_back(&v);
    }
    assert((int)v.count == 0);

    vector_free(&v);
    printf("Pass!\n");
}

void test_vector_at()
{
    printf("test_vector_at: ");

    // build vector
    Vector v = {};
    vector_init(&v, sizeof(int));

    const int N = 10;
    for (int i = 0; i < N; i++)
        vector_push_back(&v, &i);

    // check at for reading items
    for (size_t i = 0; i < N; i++)
        assert(*(int*)vector_at_const(&v, i) == (int)i);

    // check at for setting items too
    *(int*)vector_at(&v, 5) = -1;
    assert(((int*)v.items)[5] == -1);

    vector_free(&v);
    printf("Pass!\n");
}

void test_vector_move()
{
    printf("test_vector_move: ");

    // build vector
    Vector v = {};
    vector_init(&v, sizeof(int));

    const int N = 10;
    for (int i = 0; i < N; i++)
        vector_push_back(&v, &i);

    // move vector
    Vector w = {};
    vector_move(&v, &w);

    // check new vector
    assert(w.count == N);
    const int* w_items = w.items;
    for (int i = 0; i < N; i++)
        assert(w_items[i] == i);

    // check old vector is empty
    assert(v.items == NULL);
    assert(v.count == 0);

    vector_free(&w);
    printf("Pass!\n");
}

int main()
{
    test_vector_ints();
    test_vector_structs();
    test_vector_arrays();
    test_vector_reserve();
    test_vector_pop_back();
    test_vector_at();
    test_vector_move();
    return EXIT_SUCCESS;
}
