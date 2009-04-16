#ifndef _OARRAY_H
#define _OARRAY_H

/* Ordered array implementation. This provides an insertion-sorted array for
 * the kernel. It stores a generic pointer (void *) as elements, so any type
 * can be stored.
 */

typedef void* type_t;

typedef char (*lessthan_predicate_t)(type_t, type_t);

typedef struct {
    type_t *array;
    unsigned int size;
    unsigned int max_size;
    lessthan_predicate_t less_than;
} oarray_t;

char standard_lessthan_predicate(type_t a, type_t b);

oarray_t create_oarray(unsigned int max_size, lessthan_predicate_t less_than);

oarray_t place_oarray(void *addr, unsigned int max_size, 
        lessthan_predicate_t less_than);

void destroy_oarray(oarray_t *array);

void insert_oarray(type_t item, oarray_t *array);

type_t lookup_oarray(unsigned int i, oarray_t *array);

void remove_oarray(unsigned int i, oarray_t *array);

#endif
