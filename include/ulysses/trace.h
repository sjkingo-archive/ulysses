#ifndef _STACKTRACE_H
#define _STACKTRACE_H

#if __GNUC__
#define TRACE_ONCE add_trace_symbol(__func__, __builtin_return_address(0))
#else
#define TRACE_ONCE do {} while(0)
#endif

void add_trace_symbol(const char func_name[], void *addr);

void func_trace(unsigned int max_frames);

#endif
