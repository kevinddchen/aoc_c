#include "util.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const char FILENAME[] = "files/day9.txt";

static const long DAY9_PART1_ANS = 4776487744;

/**
 * 2D vector.
 */
typedef struct {
    long x;
    long y;
} Vec2i;

/**
 * Read vertices from file.
 */
void read_vertices(const char* filename, Vector* vertices)
{
    vector_init(vertices, sizeof(Vec2i));

    FILE* fp = fopen(filename, "r");
    assert(fp != NULL);

    char buff[1024] = {};
    while (fgets(buff, sizeof buff, fp) != NULL) {
        // parse "x,y,z"
        char* ptr = buff;
        const long x = util_strtol(ptr, &ptr, 0);
        const long y = util_strtol(ptr + 1, NULL, 0);

        vector_push_back(vertices, &(Vec2i){x, y});
    }

    fclose(fp);
    fp = NULL;
}

int main()
{
    Vector vertices = {};
    read_vertices(FILENAME, &vertices);

    // find max area by brute force
    long max_area = 0;
    for (size_t i = 0; i < vertices.count; i++) {
        for (size_t j = 0; j < vertices.count; j++) {
            const Vec2i* v1 = (Vec2i*)vertices.items + i;
            const Vec2i* v2 = (Vec2i*)vertices.items + j;
            const long area = (labs(v1->x - v2->x) + 1) * (labs(v1->y - v2->y) + 1);
            if (area > max_area)
                max_area = area;
        }
    }

    printf("Day 9\n");
    printf("Part 1: %ld\n", max_area);

    assert(max_area == DAY9_PART1_ANS);

    vector_free(&vertices);

    return EXIT_SUCCESS;
}
