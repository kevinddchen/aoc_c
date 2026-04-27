#include "util.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char FILENAME[] = "files/day8.txt";

static const int DAY8_PART1_ANS = 50760;
static const long DAY8_PART2_ANS = 3206508875;

// Number of junction boxes to connect
static const int NUM_CONNECTIONS = 1000;

/**
 * 3D vector of integers.
 */
typedef struct {
    long x;
    long y;
    long z;
} Vec3i;

/**
 * Edge between two vertices. Also keeps track of the square of their Euclidean distance.
 */
typedef struct {
    size_t v1_idx;
    size_t v2_idx;
    long sq_dist;
} Edge;

/**
 * Square a number.
 */
inline long square(long x)
{
    return x * x;
}

/**
 * Square of the Euclidean distance between two 3D points.
 */
inline long square_distance(const Vec3i* v1, const Vec3i* v2)
{
    return square(v1->x - v2->x) + square(v1->y - v2->y) + square(v1->z - v2->z);
}

/**
 * Sorts edges in increasing order of their `.sq_dist` parameter.
 */
int compare_edges(const void* a, const void* b)
{
    const Edge* e1 = a;
    const Edge* e2 = b;
    return (e1->sq_dist > e2->sq_dist) - (e1->sq_dist < e2->sq_dist);
}

/**
 * Sorts `size_t`s in decreasing order.
 */
int compare_size_t_decreasing(const void* a, const void* b)
{
    const long arg1 = *(const size_t*)a;
    const long arg2 = *(const size_t*)b;
    return -((arg1 > arg2) - (arg1 < arg2));
}

/**
 * Read vertices from file.
 */
void read_vertices(const char* filename, Vector* vertices)
{
    vector_init(vertices, sizeof(Vec3i));

    FILE* fp = fopen(filename, "r");
    assert(fp != NULL);

    char buff[1024] = {};
    while (fgets(buff, sizeof buff, fp) != NULL) {
        // parse "x,y,z"
        char* ptr = buff;
        Vec3i* vec = vector_emplace_back(vertices);
        vec->x = util_strtol(ptr, &ptr, 0);
        assert(*ptr == ',');
        vec->y = util_strtol(ptr + 1, &ptr, 0);
        assert(*ptr == ',');
        vec->z = util_strtol(ptr + 1, NULL, 0);
    }

    fclose(fp);
    fp = NULL;
}

/**
 * Connect two vertices given by the edge. Returns true if the resultant graph is connected, false otherwise.
 */
bool connect_edges(const Edge* edge, size_t* circuit_sizes, size_t* vertex_to_circuit, size_t num_vertices)
{
    const size_t circuit_id_1 = vertex_to_circuit[edge->v1_idx];
    const size_t circuit_id_2 = vertex_to_circuit[edge->v2_idx];
    if (circuit_id_1 == circuit_id_2)
        return false;

    // merge circuit_id_2 into circuit_id_1
    circuit_sizes[circuit_id_1] += circuit_sizes[circuit_id_2];
    circuit_sizes[circuit_id_2] = 0;
    for (size_t v_idx = 0; v_idx < num_vertices; v_idx++) {
        if (vertex_to_circuit[v_idx] == circuit_id_2)
            vertex_to_circuit[v_idx] = circuit_id_1;
    }
    // if connected, new circuit size should equal number of vertices
    return circuit_sizes[circuit_id_1] == num_vertices;
}

int main()
{
    Vector vertices = {};  // tracks list of vertices
    read_vertices(FILENAME, &vertices);

    // compute distances between each pair of vertices
    Vector edges = {};
    vector_init(&edges, sizeof(Edge));
    vector_reserve(&edges, (vertices.count * (vertices.count - 1)) / 2);
    for (size_t v1_idx = 0; v1_idx < vertices.count; v1_idx++) {
        for (size_t v2_idx = v1_idx + 1; v2_idx < vertices.count; v2_idx++) {
            Edge* edge = vector_emplace_back(&edges);
            edge->v1_idx = v1_idx;
            edge->v2_idx = v2_idx;
            edge->sq_dist = square_distance(vector_at_const(&vertices, v1_idx), vector_at_const(&vertices, v2_idx));
        }
    }

    // sort distances in increasing order
    qsort(edges.items, edges.count, edges.item_size, compare_edges);

    // initialize graph data structures
    size_t* circuit_sizes = malloc(vertices.count * sizeof(size_t));  // tracks size of each circuit
    assert(circuit_sizes != NULL);
    size_t* vertex_to_circuit = malloc(vertices.count * sizeof(size_t));  // tracks vertex idx -> circuit id mapping
    assert(vertex_to_circuit != NULL);
    for (size_t i = 0; i < vertices.count; i++) {
        // initially, every vertex is a circuit
        circuit_sizes[i] = 1;
        vertex_to_circuit[i] = i;
    }

    // connect the closest `NUM_CONNECTIONS` pairs of vertices
    for (size_t i = 0; i < NUM_CONNECTIONS; i++)
        connect_edges(vector_at_const(&edges, i), circuit_sizes, vertex_to_circuit, vertices.count);

    // find three largest circuit sizes
    long p1_ans;
    {
        size_t* circuit_sizes_copy = malloc(vertices.count * sizeof(size_t));
        assert(circuit_sizes_copy != NULL);
        memcpy(circuit_sizes_copy, circuit_sizes, vertices.count * sizeof(size_t));

        // sort circuit sizes in decreasing order. overkill, but simple enough.
        qsort(circuit_sizes_copy, vertices.count, sizeof(size_t), compare_size_t_decreasing);

        p1_ans = circuit_sizes_copy[0] * circuit_sizes_copy[1] * circuit_sizes_copy[2];

        free(circuit_sizes_copy);
        circuit_sizes_copy = NULL;
    }

    // continue to connect pairs of vertices until all vertices are connected
    long p2_ans;
    {
        size_t i = NUM_CONNECTIONS;
        while (!connect_edges(vector_at_const(&edges, i), circuit_sizes, vertex_to_circuit, vertices.count))
            i++;

        const Edge* last_edge = vector_at_const(&edges, i);
        const Vec3i* v1 = vector_at_const(&vertices, last_edge->v1_idx);
        const Vec3i* v2 = vector_at_const(&vertices, last_edge->v2_idx);

        p2_ans = v1->x * v2->x;
    }

    printf("Day 8\n");
    printf("Part 1: %ld\n", p1_ans);
    printf("Part 2: %ld\n", p2_ans);

    assert(p1_ans == DAY8_PART1_ANS);
    assert(p2_ans == DAY8_PART2_ANS);

    free(vertex_to_circuit);
    vertex_to_circuit = NULL;
    free(circuit_sizes);
    circuit_sizes = NULL;
    vector_free(&edges);
    vector_free(&vertices);

    return EXIT_SUCCESS;
}
