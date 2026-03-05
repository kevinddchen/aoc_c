#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const size_t VECTOR_INITIAL_CAPACITY = 8;

void vector_init(Vector* v, size_t item_size)
{
    assert(item_size > 0);
    v->items = NULL;
    v->count = 0;
    v->capacity = 0;
    v->item_size = item_size;
}

void vector_reserve(Vector* v, size_t new_capacity)
{
    if (new_capacity <= v->capacity)
        return;

    assert(new_capacity <= SIZE_MAX / v->item_size);
    void* new_items = realloc(v->items, new_capacity * v->item_size);
    assert(new_items != NULL);
    v->items = new_items;
    v->capacity = new_capacity;
}

/**
 * Increase the capacity of a vector.
 */
static void expand_capacity(Vector* v)
{
    size_t new_capacity;
    if (v->capacity == 0) {
        new_capacity = VECTOR_INITIAL_CAPACITY;
    } else {
        assert(v->capacity <= SIZE_MAX / 2);
        new_capacity = v->capacity * 2;
    }
    vector_reserve(v, new_capacity);
}

void vector_push_back(Vector* v, const void* item)
{
    if (v->count == v->capacity)
        expand_capacity(v);

    // copy item into vector
    memcpy((char*)v->items + v->count * v->item_size, item, v->item_size);
    v->count++;
}

void* vector_emplace_back(Vector* v)
{
    if (v->count == v->capacity)
        expand_capacity(v);

    v->count++;
    return (char*)v->items + (v->count - 1) * v->item_size;
}

void vector_pop_back(Vector* v)
{
    assert(v->count > 0);
    v->count--;
}

void* vector_at(Vector* v, size_t index)
{
    assert(index < v->count);
    return (char*)v->items + index * v->item_size;
}

const void* vector_at_const(const Vector* v, size_t index)
{
    assert(index < v->count);
    return (char*)v->items + index * v->item_size;
}

void vector_free(Vector* v)
{
    free(v->items);
    v->items = NULL;
    v->count = 0;
    v->capacity = 0;
    v->item_size = 0;
}

void vector_free_with(Vector* v, void (*destructor)(void*))
{
    if (destructor) {
        for (size_t i = 0; i < v->count; i++) {
            void* ptr = (char*)v->items + i * v->item_size;
            destructor(ptr);
        }
    }

    vector_free(v);
}

static void free_array(void* ptr)
{
    free(*(void**)ptr);
}

void vector_free_arrays(Vector* v)
{
    assert(v->item_size == sizeof(void*));
    vector_free_with(v, free_array);
}

void vector_move(Vector* src, Vector* dest)
{
    if (src == dest)
        return;

    dest->items = src->items;
    dest->count = src->count;
    dest->capacity = src->capacity;
    dest->item_size = src->item_size;

    src->items = NULL;
    src->count = 0;
    src->capacity = 0;
    src->item_size = 0;
}
