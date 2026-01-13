#include "util.h"
#include "vector.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const char FILENAME[] = "files/day8.txt";

// Number of junction boxes to connect
static const int NUM_CONNECTIONS = 1000;

static const int DAY8_PART1_ANS = 50760;

typedef struct {
    double x;
    double y;
    double z;
} Vector3d;

typedef struct {
    double distance;
    size_t v1_idx;
    size_t v2_idx;
} Edge;

inline double square(double x)
{
    return x * x;
}

inline double distance(const Vector3d* v1, const Vector3d* v2)
{
    return sqrt(square(v1->x - v2->x) + square(v1->y - v2->y) + square(v1->z - v2->z));
}

/**
 * Sorts edges in increasing order according to their `.distance` parameter.
 */
int compare_edges(const void* a, const void* b)
{
    const Edge* e1 = a;
    const Edge* e2 = b;
    return (e1->distance > e2->distance) - (e1->distance < e2->distance);
}

/**
 * Sorts ints in decreasing order.
 */
int compare_ints_decreasing(const void* a, const void* b)
{
    const int arg1 = *(const int*)a;
    const int arg2 = *(const int*)b;
    return -((arg1 > arg2) - (arg1 < arg2));
}

int main()
{
    FILE* fp = fopen(FILENAME, "r");
    assert(fp != NULL);

    // read vertices from file
    Vector vertices = {};  // tracks list of vertices
    vector_init(&vertices, sizeof(Vector3d));

    char buff[1024] = {};
    while (fgets(buff, sizeof buff, fp) != NULL) {
        // parse "x,y,z"
        char* ptr = buff;
        const long x = util_strtol(ptr, &ptr, 0);
        const long y = util_strtol(ptr + 1, &ptr, 0);
        const long z = util_strtol(ptr + 1, NULL, 0);

        vector_push_back(&vertices, &((Vector3d){x, y, z}));
    }

    // printf("Vertices\n");
    // for (size_t i = 0; i < vertices.count; i++) {
    //     const Vector3d* v = (Vector3d*)vertices.items + i;
    //     printf("%zu: %.0f %.0f %.0f\n", i, v->x, v->y, v->z);
    // }
    // printf("\n");

    // compute distances between each pair of vertices
    Vector distance_edges = {};  // tracks distances
    vector_init(&distance_edges, sizeof(Edge));
    vector_reserve(&distance_edges, (vertices.count * (vertices.count - 1)) / 2);
    for (size_t v1_idx = 0; v1_idx < vertices.count; v1_idx++) {
        for (size_t v2_idx = v1_idx + 1; v2_idx < vertices.count; v2_idx++) {
            const Vector3d* v1 = (Vector3d*)vertices.items + v1_idx;
            const Vector3d* v2 = (Vector3d*)vertices.items + v2_idx;
            vector_push_back(&distance_edges, &(Edge){distance(v1, v2), v1_idx, v2_idx});
        }
    }

    // sort distances in increasing order
    qsort(distance_edges.items, distance_edges.count, distance_edges.item_size, compare_edges);

    // printf("Closest pairs\n");
    // for (size_t i = 0; i < NUM_EDGES; i++) {
    //     const Edge* e = (Edge*)distance_edges.items + i;
    //     printf("#%zu: %zu %zu\n", i, e->v1_idx, e->v2_idx);
    // }
    // printf("\n");

    // connect the closest `NUM_CONNECTIONS` pairs of vertices
    int* circuit_sizes = malloc(vertices.count * sizeof(int));            // tracks size of each circuit
    size_t* vertex_to_circuit = malloc(vertices.count * sizeof(size_t));  // tracks vertex idx -> circuit id mapping
    for (size_t i = 0; i < vertices.count; i++) {
        // initially, every vertex is a circuit
        circuit_sizes[i] = 1;
        vertex_to_circuit[i] = i;
    }

    for (size_t i = 0; i < NUM_CONNECTIONS; i++) {
        const Edge* edge = ((Edge*)distance_edges.items) + i;
        const size_t circuit_id_1 = vertex_to_circuit[edge->v1_idx];
        const size_t circuit_id_2 = vertex_to_circuit[edge->v2_idx];
        if (circuit_id_1 == circuit_id_2)
            continue;
        // merge circuit_id_2 into circuit_id_1
        circuit_sizes[circuit_id_1] += circuit_sizes[circuit_id_2];
        circuit_sizes[circuit_id_2] = 0;
        for (size_t v_idx = 0; v_idx < vertices.count; v_idx++) {
            if (vertex_to_circuit[v_idx] == circuit_id_2)
                vertex_to_circuit[v_idx] = circuit_id_1;
        }

        // printf("Iteration #%zu\n", i);
        // for (size_t i = 0; i < vertices.count; i++)
        //     printf("%d ", circuit_sizes[i]);
        // printf("\n");
        // for (size_t i = 0; i < vertices.count; i++)
        //     printf("%zu ", vertex_to_circuit[i]);
        // printf("\n\n");
    }

    // sort circuit sizes in decreasing order
    qsort(circuit_sizes, vertices.count, sizeof(int), compare_ints_decreasing);

    const int p1_ans = circuit_sizes[0] * circuit_sizes[1] * circuit_sizes[2];

    printf("Day 8\n");
    printf("Part 1: %d\n", p1_ans);

    assert(p1_ans == DAY8_PART1_ANS);

    free(vertex_to_circuit);
    vertex_to_circuit = NULL;
    free(circuit_sizes);
    circuit_sizes = NULL;
    vector_free(&distance_edges);
    vector_free(&vertices);
    fclose(fp);
    fp = NULL;

    return EXIT_SUCCESS;
}
