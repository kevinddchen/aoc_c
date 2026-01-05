#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * Type-erased dynamic array.
 */
typedef struct {
    void* items;
    size_t count;
    size_t capacity;
    size_t item_size;
} Vector;

/**
 * Initialize an empty vector.
 * @param v Vector.
 * @param item_size Size of vector item, in bytes.
 */
static inline void vector_init(Vector* v, size_t item_size) {
    v->items = NULL;
    v->count = 0;
    v->capacity = 0;
    v->item_size = item_size;
}

/**
 * Increase capacity of a vector.
 * @param v Vector.
 * @param new_capacity New capacity.
 */
static inline void vector_reserve(Vector* v, size_t new_capacity) {
    if (new_capacity > v->capacity) {
        void* new_items = realloc(v->items, new_capacity * v->item_size);
        assert(new_items);
        v->items = new_items;
        v->capacity = new_capacity;
    }
}

/**
 * Append an item to the end of a vector.
 * @param v Vector.
 * @param item Pointer to item to be appended.
 */
static inline void vector_push_back(Vector* v, const void* item) {
    if (v->count == v->capacity) {
        const size_t new_capacity = v->capacity ? v->capacity * 2 : 8;
        vector_reserve(v, new_capacity);
    }

    // copy item into vector
    memcpy((char*)v->items + v->count * v->item_size, item, v->item_size);
    v->count++;
}

/**
 * Deallocate vector's memory.
 * @param v Vector.
 */
static inline void vector_free(Vector* v) {
    free(v->items);
    v->items = NULL;
    v->count = 0;
    v->capacity = 0;
}
