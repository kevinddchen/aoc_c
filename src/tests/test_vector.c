#include "vector.h"

#include <stdio.h>

void test_vector_ints()
{
    printf("test_vector_ints: ");

    // build vector of squares 0, 1, 4, 9, ...
    Vector v = {};
    vector_init(&v, sizeof(int));

    const int N = 100;
    for (int i = 0; i < N; i++) {
        const int square = i * i;
        vector_push_back(&v, &square);
    }

    // check vector is as expected
    assert(v.count == N);
    for (int i = 0; i < N; i++) {
        assert(((int*)v.items)[i] == i * i);
    }

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
        const Triple triple = {i, i * i, i * i * i};
        vector_push_back(&v, &triple);
    }

    // check vector is as expected
    assert(v.count == N);
    for (int i = 0; i < N; i++) {
        assert(((Triple*)v.items)[i].number == i);
        assert(((Triple*)v.items)[i].square == i * i);
        assert(((Triple*)v.items)[i].cube == i * i * i);
    }

    vector_free(&v);
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
        vector_push_back(&v, &i);
        assert(v.capacity == N);
    }

    // calling with smaller capacity does nothing
    vector_reserve(&v, 0);
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

    const int N = 100;
    for (int i = 0; i < N; i++) {
        vector_push_back(&v, &i);
    }

    // check pop
    for (int i = N - 1; i >= 0; i--) {
        assert(((int*)v.items)[i] == i);
        vector_pop_back(&v);
        assert((int)v.count == i);
    }

    vector_free(&v);
    printf("Pass!\n");
}

void test_vector_move()
{
    printf("test_vector_move: ");

    // build vector
    Vector v = {};
    vector_init(&v, sizeof(int));

    const int N = 5;
    for (int i = 0; i < N; i++) {
        vector_push_back(&v, &i);
    }

    // move vector
    Vector w = {};
    vector_move(&v, &w);

    // check new vector
    assert(w.count == N);
    for (int i = 0; i < N; i++) {
        assert(((int*)w.items)[i] == i);
    }

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
    test_vector_reserve();
    test_vector_pop_back();
    test_vector_move();
    return EXIT_SUCCESS;
}
