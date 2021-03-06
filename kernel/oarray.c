/* kernel/oarray.c - ordered array implementation used for the heap
 * part of Ulysses, a tiny operating system
 *
 * Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ulysses/kheap.h>
#include <ulysses/oarray.h>

#include <string.h>

char standard_lessthan_predicate(type_t a, type_t b)
{
    if (a < b) {
        return 1;
    } else {
        return 0;
    }
}

oarray_t create_oarray(unsigned int max_size, lessthan_predicate_t less_than)
{
    oarray_t a;
    a.array = (void *)kmalloc(max_size * sizeof(type_t));
    
    memset(a.array, 0, max_size * sizeof(type_t));
    a.size = 0;
    a.max_size = max_size;
    a.less_than = less_than;

    return a;
}

oarray_t place_oarray(void *addr, unsigned int max_size, 
        lessthan_predicate_t less_than)
{
    oarray_t a;

    a.array = (type_t *)addr;
    memset(a.array, 0, max_size * sizeof(type_t));
    a.size = 0;
    a.max_size = max_size;
    a.less_than = less_than;

    return a;
}

void destroy_oarray(oarray_t *array)
{
    kfree(array->array);
}

void insert_oarray(type_t item, oarray_t *array)
{
    unsigned int i = 0;

    while ((i < array->size) && (array->less_than(array->array[i], item))) {
        i++;
    }

    if (i == array->size) {
        /* Add to end of array */
        array->array[array->size++] = item;

    } else {
        type_t tmp = array->array[i];
        array->array[i] = item;
        while (i < array->size) {
            i++;
            type_t tmp2 = array->array[i];
            array->array[i] = tmp;
            tmp = tmp2;
        }
        array->size++;
    }
}

type_t lookup_oarray(unsigned int i, oarray_t *array)
{
    return array->array[i];
}

void remove_oarray(unsigned int i, oarray_t *array)
{
    while (i < array->size) {
        array->array[i] = array->array[i+1];
        i++;
    }
    array->size--;
}
