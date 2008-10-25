
arch_sources = [ 'arch/x86/%s' % file for file in [ 'startup.c', 'halt.c', 
        'util.c', 'timer.c', 'gdt.c', 'flush.s', 'paging.c', 'screen.c', 
        'loader.s' ] ] +  \
        [ 'arch/x86/isr/%s' % file for file in [ 'idt.c', 'interrupt.s', 
        'isr.c' ] ]

kern_sources = [ 'kernel/%s' % file for file in [ 'main.c', 'kprintf.c', 
        'proc.c', 'shutdown.c', 'vt.c', 'util.c' ] ]

lib_sources = [ 'lib/%s' % file for file in [ 'itoa.c', 'string.c' ] ]

linker_script = 'arch/x86/linker.ld'

env = Environment(
    AS = 'nasm', 
    CC = 'gcc', 
    CFLAGS = '-m32 -g -Wall -Wextra -isystem include', 
    LINKFLAGS = '-m32 -nostdlib -nostartfiles -nodefaultlibs -T %s' % 
            linker_script,
    ASFLAGS = '-f elf',
)

env.Decider('timestamp-match')
env.Program('arch/x86/boot/kernel', arch_sources + kern_sources + lib_sources)

