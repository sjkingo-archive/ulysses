#ifndef _ULYSSES_OARRAY_H
#define _ULYSSES_OARRAY_H

/* Ordered array implementation. This provides an insertion-sorted array for
 * the kernel. It stores a generic pointer (void *) as elements, so any type
 * can be stored.
 */

/* A generic type */
typedef void* type_t;

/* Function pointer for determining if a is less than b */
typedef char (*lessthan_predicate_t)(type_t, type_t);

/* An ordered array */
typedef struct {
    type_t *array;
    unsigned int size;
    unsigned int max_size;
    lessthan_predicate_t less_than;
} oarray_t;

/* standard_lessthan_predicate()
 *  Custom sorting function for characters.
 */
char standard_lessthan_predicate(type_t a, type_t b);

/* create_oarray()
 *  Create a new ordered array of the given size and a custom predicate
 *  function for determining sorting order.
 */
oarray_t create_oarray(unsigned int max_size, lessthan_predicate_t less_than);

/* place_oarray()
 *  Place a new ordered array at the given (pre-allocated) memory address.
 */
oarray_t place_oarray(void *addr, unsigned int max_size, 
        lessthan_predicate_t less_than);

/* destroy_oarray()
 *  Destroy an ordered array and all of its elements.
 */
void destroy_oarray(oarray_t *array);

/* insert_oarray()
 *  Insert the given element into an ordered array in the custom sorted order.
 */
void insert_oarray(type_t item, oarray_t *array);

/* lookup_oarray()
 *  Return the element in position i of the given ordered array.
 */
type_t lookup_oarray(unsigned int i, oarray_t *array);

/* remove_oarray()
 *  Remove the element in position i of the given ordered array.
 */
void remove_oarray(unsigned int i, oarray_t *array);

#endif
