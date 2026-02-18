#include "vector.h"

#include <assert.h>
#include <stdio.h>

void test_vector_ints()
{
    printf("test_vector_ints: ");

    // build vector of squares 0, 1, 4, 9, ...
    Vector v = {};
    vector_init(&v, sizeof(int));

    static const int N = 100;

    for (int i = 0; i < N; i++) {
        const int square = i * i;
        vector_push_back(&v, &square);
    }

    // check vector is as expected
    assert(v.count == N);

    for (int i = 0; i < N; i++) {
        assert(((int*)v.items)[i] == i * i);
    }

    // check cleanup
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

    static const int N = 100;

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

    // check cleanup
    vector_free(&v);

    printf("Pass!\n");
}

void test_vector_reserve()
{
    printf("test_vector_reserve: ");

    Vector v = {};
    vector_init(&v, sizeof(v));

    // reserve, then check that capacity does not change
    static const int N = 100;

    vector_reserve(&v, N);
    assert(v.capacity == N);

    for (int i = 0; i < N; i++) {
        vector_push_back(&v, &i);
        assert(v.capacity == N);
    }

    // calling with smaller capacity does not do anything
    vector_reserve(&v, 0);
    assert(v.capacity == N);

    printf("Pass!\n");
}

void test_vector_pop_back()
{
    printf("test_vector_pop_back: ");

    // build vector
    Vector v = {};
    vector_init(&v, sizeof(int));

    static const int N = 100;

    for (int i = 0; i < N; i++) {
        vector_push_back(&v, &i);
    }

    // check pop
    for (int i = N - 1; i >= 0; i--) {
        assert(((int*)v.items)[i] == i);
        vector_pop_back(&v);
        assert((int)v.count == i);
    }

    printf("Pass!\n");
}

int main()
{
    test_vector_ints();
    test_vector_structs();
    test_vector_reserve();
    test_vector_pop_back();
}
