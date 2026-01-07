#include "vector.h"

#include <assert.h>
#include <stdio.h>

void test_vector_ints()
{
    printf("test_vector_ints: ");

    // build vector of squares 0, 1, 4, 9, ...
    Vector v = {};
    vector_init(&v, sizeof(int));

    const int N = 10;

    for (int i = 0; i < N; i++) {
        const int square = i * i;
        vector_push_back(&v, &square);
    }

    // check vector is as expected
    assert(v.count == N);

    for (int i = 0; i < N; i++) {
        assert(((int*)v.items)[i] == i * i);
    }

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

    const int N = 10;

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

    printf("Pass!\n");
}

int main()
{
    test_vector_ints();
    test_vector_structs();
}
