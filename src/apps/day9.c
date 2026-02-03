#include "util.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const char FILENAME[] = "files/day9.txt";

static const long DAY9_PART1_ANS = 4776487744;
static const long DAY9_PART2_ANS = 1560299548;

static const long UNSET_DEPTH = -1;

/**
 * 2D vector.
 */
typedef struct {
    long x;
    long y;
} Vec2i;

/**
 * Compute dot-product of two 2D vectors, `a . b`.
 */
inline long dot(const Vec2i* a, const Vec2i* b)
{
    return a->x * b->x + a->y * b->y;
}

/**
 * Compute cross-product of two 2D vectors, `a x b`.
 */
inline long cross(const Vec2i* a, const Vec2i* b)
{
    return a->x * b->y - a->y * b->x;
}

/**
 * Normalize an axis-aligned vector `v`. Returns the norm of `v` (before normalization).
 */
inline long normalize_axis_aligned(Vec2i* v)
{
    assert((v->x == 0) ^ (v->y == 0));
    long norm = {};
    if (v->x == 0) {
        norm = labs(v->y);
        v->y /= norm;
    } else {
        norm = labs(v->x);
        v->x /= norm;
    }
    return norm;
}

/**
 * Compute the unit heading from a to b. Heading must be axis-aligned. Returns the norm of `b - a`.
 */
inline long get_unit_heading(const Vec2i* a, const Vec2i* b, Vec2i* heading)
{
    heading->x = b->x - a->x;
    heading->y = b->y - a->y;
    return normalize_axis_aligned(heading);
}

/**
 * Given a contour of axis-aligned edges, compute the turn angle at each vertex: +1 for a turn 90 degrees left, and -1
 * for a turn 90 degrees right.
 * @param contour List of Vec2i vertices that make up the contour. Contour must be closed, where the last element
 * connects to the first element by an edge. Contour edges must also be axis-aligned.
 * @param turns Output list of int turns, same length as `contour`.
 */
void compute_turns(const Vector* contour, Vector* turns)
{
    assert(contour->count > 1);
    vector_reserve(turns, contour->count);

    Vec2i prev_heading = {};
    get_unit_heading((Vec2i*)contour->items + contour->count - 1, (Vec2i*)contour->items, &prev_heading);

    for (size_t i = 0; i < contour->count; i++) {
        Vec2i heading = {};
        get_unit_heading((Vec2i*)contour->items + i, (Vec2i*)contour->items + (i + 1) % contour->count, &heading);

        // cross-product gives us direction of the 90-degree turn
        const long turn = cross(&prev_heading, &heading);
        assert(turn == 1 || turn == -1);
        vector_push_back(turns, &turn);

        prev_heading.x = heading.x;
        prev_heading.y = heading.y;
    }
}

/**
 * Given a contour of axis-aligned edges, compute the winding number.
 * @param turns List of int turns, output of `complete_turns`.
 */
int compute_winding_number(const Vector* turns)
{
    int total_turn = 0;  // tracks 4 times the winding number, i.e. the number of 90-degree turns

    for (size_t i = 0; i < turns->count; i++) {
        total_turn += ((int*)turns->items)[i];
    }

    assert(total_turn % 4 == 0);  // winding must be integral in a closed contour
    return total_turn / 4;
}

/**
 * Given a point v and an axis-aligned unit vector dv, render the distance along the ray to the contour.
 * @param v 2d vector.
 * @param dv Unit 2d vector; must be axis-aligned.
 * @param contour List of Vec2i vertices that make up the contour. Contour must be closed, where the last element
 * connects to the first element by an edge. Contour edges must also be axis-aligned.
 * @param turns List of int turns, same length as `contour`.
 * @param winding_number The winding number of the contour.
 * @returns Rendered depth, or UNSET_DEPTH if no intersection with the contour.
 */
long render_depth(const Vec2i* v, const Vec2i* dv, const Vector* contour, const Vector* turns, int winding_number)
{
    assert(winding_number == 1 || winding_number == -1);

    long depth = UNSET_DEPTH;

    // iterate over contour edges
    for (size_t i = 0; i < contour->count; i++) {
        const Vec2i* w0 = (Vec2i*)contour->items + i;
        const Vec2i* w1 = (Vec2i*)contour->items + (i + 1) % contour->count;

        Vec2i dw = {};
        const long w_dist = get_unit_heading(w0, w1, &dw);

        // skip if parallel or hitting backface
        if (cross(dv, &dw) * winding_number <= 0)
            continue;

        // dv . dw = 0 since they are parallel
        assert(dot(dv, &dw) == 0);

        // solve for intersection
        // v + a * dv = w0 + b * dw
        // a = -v . dv + w0 . dv
        // b =  v . dw - w0 . dw
        const long a = -dot(v, dv) + dot(w0, dv);
        const long b = dot(v, &dw) - dot(w0, &dw);

        // skip if wrong side of ray
        if (a < 0)
            continue;

        // skip if ray doesn't intersect contour edge
        if (b < 0 || b > w_dist)
            continue;

        // special case 1: intersects start of contour edge
        if (b == 0) {
            const int turn = ((int*)turns->items)[i];
            // skip if right turn (for positive winding)
            if (turn * winding_number < 0)
                continue;
        }
        // special case 2: intersects end of contour edge
        if (b == w_dist) {
            const int turn = ((int*)turns->items)[(i + 1) % turns->count];
            // skip if right turn (for positive winding)
            if (turn * winding_number < 0)
                continue;
        }

        if (depth == UNSET_DEPTH || a < depth)
            depth = a;
    }

    return depth;
}

/**
 * Returns true if the (axis-aligned) vector from a to b does not intersect the contour.
 * @param a 2d vector.
 * @param b 2d vector.
 * @param contour List of Vec2i vertices that make up the contour. Contour must be closed, where the last element
 * connects to the first element by an edge. Contour edges must also be axis-aligned.
 * @param turns List of int turns, same length as `contour`.
 * @param winding_number The winding number of the contour.
 */
bool check_sightline(const Vec2i* a, const Vec2i* b, const Vector* contour, const Vector* turns, int winding_number)
{
    Vec2i heading = {};
    const long dist = get_unit_heading(a, b, &heading);
    const long depth = render_depth(a, &heading, contour, turns, winding_number);
    return (depth != UNSET_DEPTH && dist <= depth);
}

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
        // parse "x,y"
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
            const Vec2i* vi = (Vec2i*)vertices.items + i;
            const Vec2i* vj = (Vec2i*)vertices.items + j;
            const long area = (labs(vi->x - vj->x) + 1) * (labs(vi->y - vj->y) + 1);
            if (area > max_area)
                max_area = area;
        }
    }

    Vector turns = {};
    vector_init(&turns, sizeof(int));
    compute_turns(&vertices, &turns);
    const int winding_number = compute_winding_number(&turns);

    // iterate over all pairs of vertices
    long max_interior_area = 0;
    for (size_t i = 0; i < vertices.count; i++) {
        for (size_t j = i + 1; j < vertices.count; j++) {
            const Vec2i* vi = (Vec2i*)vertices.items + i;
            const Vec2i* vj = (Vec2i*)vertices.items + j;

            // short-circuit: skip if area is smaller
            const long area = (labs(vi->x - vj->x) + 1) * (labs(vi->y - vj->y) + 1);
            if (area <= max_interior_area)
                continue;

            if (vi->x == vj->x || vi->y == vj->y) {
                // line case
                if (!check_sightline(vi, vj, &vertices, &turns, winding_number))
                    continue;
            } else {
                // square case
                const Vec2i va = {vi->x, vj->y};
                const Vec2i vb = {vj->x, vi->y};
                if (!(check_sightline(vi, &va, &vertices, &turns, winding_number) &&
                      check_sightline(vi, &vb, &vertices, &turns, winding_number) &&
                      check_sightline(vj, &va, &vertices, &turns, winding_number) &&
                      check_sightline(vj, &vb, &vertices, &turns, winding_number)))
                    continue;
            }

            max_interior_area = area;
        }
    }

    printf("Day 9\n");
    printf("Part 1: %ld\n", max_area);
    printf("Part 2: %ld\n", max_interior_area);

    assert(max_area == DAY9_PART1_ANS);
    assert(max_interior_area == DAY9_PART2_ANS);

    vector_free(&turns);
    vector_free(&vertices);

    return EXIT_SUCCESS;
}
