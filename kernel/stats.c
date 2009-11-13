/* kernel/stats.c - process statistics and performance monitoring
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

#include <ulysses/stats.h>
#include <ulysses/task.h>

#define MAX_PID 1024
static struct statistics tasks[MAX_PID];

void new_stats(pid_t pid)
{
    if (pid >= MAX_PID) {
        return;
    }
    tasks[pid].in_use = TRUE;
    tasks[pid].cpu_time = 0;
    tasks[pid].virtual_mem = 0;
}

void destroy_stats(pid_t pid)
{
    if (pid >= MAX_PID) {
        return;
    }
    tasks[pid].in_use = FALSE;    
}

inline void stats_update_cpu_time(void)
{
    pid_t pid = do_getpid();
    if (pid >= MAX_PID) {
        return;
    }
    if (tasks[pid].in_use) {
        tasks[pid].cpu_time++;
    }
}

inline time_t stats_get_cpu_time(pid_t pid)
{
    if (pid >= MAX_PID) {
        return;
    }
    if (tasks[pid].in_use) {
        return tasks[pid].cpu_time;
    }
}

inline void stats_update_virtual_mem(unsigned int bytes)
{
    pid_t pid = do_getpid();
    if (pid >= MAX_PID) {
        return;
    }
    if (tasks[pid].in_use) {
        tasks[pid].virtual_mem += bytes;
    }
}

inline unsigned long stats_get_virtual_mem(pid_t pid)
{
    if (pid >= MAX_PID) {
        return;
    }
    if (tasks[pid].in_use) {
        return tasks[pid].virtual_mem;
    }
}
