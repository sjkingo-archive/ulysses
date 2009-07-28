/* kernel/trace.c - stacktrace for last called functions
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
#include <ulysses/kprintf.h>
#include <ulysses/trace.h>

#include <string.h>

static symbol_t *head = NULL;

/* insert_sort()
 *  Inserts the given element into the linked list in increasing order of 
 *  address, using insertion sort.
 */
static void insert_sort(symbol_t *s)
{
    symbol_t *curr = head;
    while (curr != NULL) {
        if (s->addr <= curr->addr) {
            s->next = curr;
            curr->prev->next = s;
            s->prev = curr->prev;
            curr->prev = s;
            return;
        } else {
            if (curr->next == NULL) {
                curr->next = s;
                s->prev = curr;
                return;
            }
        }
        curr = curr->next;
    }
}

symbol_t *get_closest_symbol(void *addr)
{
    symbol_t *s = head;
    while (s != NULL) {
        if (addr > s->addr && addr < s->next->addr) {
            return s;
        }
        s = s->next;
    }
    return NULL;
}

symbol_t *get_trace_symbol(const char *func_name)
{
    symbol_t *s = head;
    while (s != NULL) {
        if (strcmp(s->name, func_name) == 0) {
            return s;
        }
        s = s->next;
    }
    return NULL;
}

void add_trace_symbol(const char func_name[], void *addr)
{
    symbol_t *s = kmalloc(sizeof(symbol_t));
    s->name = func_name;
    s->addr = addr;
    s->prev = NULL;
    s->next = NULL;

    if (head == NULL) {
        head = s;
    } else {
        insert_sort(s);
    }
}

void dump_syms(void)
{
    symbol_t *s = head;
    while (s != NULL) {
        kprintf("%p %s\n", s->addr, s->name);
        s = s->next;
    }
}
