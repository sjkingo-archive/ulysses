output_binary = 'arch/x86/boot/kernel'
subdirs = ['arch/x86', 'kernel', 'lib']

env = Environment(
    AS = 'nasm', 
    CC = 'gcc', 
    CFLAGS = '-m32 -g -O -Wall -Wextra -isystem include -fno-builtin', 
    LINKFLAGS = '-m32 -nostdinc -nostdlib -nostartfiles -nodefaultlibs -T ' \
            'arch/x86/linker.ld arch/x86/loader.o', # we cheat and add loader
    ASFLAGS = '-f elf',
)
env.Decider('timestamp-match')
Export('env')

# Compile the kernel from subdirs and link
objs = SConscript(dirs=subdirs)
env.Program(output_binary, objs)

# Other programs that need to be build
SConscript(dirs='init')
