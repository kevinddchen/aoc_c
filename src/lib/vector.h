// clang-format Language: C

#pragma once

#include <stdlib.h>

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
void vector_init(Vector* v, size_t item_size);

/**
 * Increase capacity of a vector. Does nothing if `new_capacity` is not greater than the current capacity.
 *
 * NOTE: Calling this function may modify the `v->items` pointer, invalidating old pointers.
 *
 * @param v Vector.
 * @param new_capacity New capacity.
 */
void vector_reserve(Vector* v, size_t new_capacity);

/**
 * Append a copy of an item to the end of a vector.
 *
 * NOTE: Calling this function may modify the `v->items` pointer, invalidating old pointers.
 *
 * @param v Vector.
 * @param item Pointer to item to be copied.
 */
void vector_push_back(Vector* v, const void* item);

/**
 * Append a new item to the end of a vector.
 *
 * NOTE: Calling this function may modify the `v->items` pointer, invalidating the old pointer.
 *
 * @param v Vector.
 * @returns Pointer to new item.
 */
void* vector_emplace_back(Vector* v);

/**
 * Remove the last item from a vector. Raises an error if the vector is empty.
 * @param v Vector.
 */
void vector_pop_back(Vector* v);

/**
 * Access item at the specified index, with bounds checking.
 * @param v Vector.
 * @param index Index of item.
 * @returns Pointer to item.
 */
void* vector_at(Vector* v, size_t index);

/**
 * Access item at the specified index, with bounds checking.
 * @param v Vector.
 * @param index Index of item.
 * @returns pointer to item.
 */
const void* vector_at_const(const Vector* v, size_t index);

/**
 * Deallocate vector's memory.
 * @param v Vector.
 */
void vector_free(Vector* v);

/**
 * Deallocate vector's memory, given a destructor to be called on every item.
 * @param v Vector.
 * @param destructor Function that will be called on every item of `v`. If NULL, does nothing.
 */
void vector_free_with(Vector* v, void (*destructor)(void*));

/**
 * Deallocate vector's memory, where `free` is to be called on every item.
 * @param v Vector.
 */
void vector_free_arrays(Vector* v);

/**
 * Move one vector's contents into another vector.
 * @param src Vector that will be moved. After move, `src` will be zeroed out.
 * @param dest Vector will be moved here. Should be uninitialized or freed prior to move.
 */
void vector_move(Vector* src, Vector* dest);
