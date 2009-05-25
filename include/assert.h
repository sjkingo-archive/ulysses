#ifndef _ASSERT_H
#define _ASSERT_H

/* Copyright (C) 1991,1992,1994-2001,2003,2004,2007
 * Free Software Foundation, Inc.
 * This file is part of the GNU C Library.
 *
 * The GNU C Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 * 
 *
 * Adapted for Ulysses by Sam Kingston <sam@sjkwi.com.au, 2009.
 */

/* These are included in case the calling module(s) do not have them */
#include <ulysses/kprintf.h>
#include <ulysses/shutdown.h>

static inline void __assert_fail(const char *a, const char *file, 
        unsigned int line, const char *func)
{
    kprintf("Assertion `%s` failed at %s:%d:%s()\n", a, file, line, func);
    panic("Assertion failed");
}

/* assert()
 *  Evaluates the given expression and panics if false.
 */
#define assert(expr) ((expr) ? : __assert_fail(#expr, __FILE__, __LINE__, \
            __func__))

#endif
