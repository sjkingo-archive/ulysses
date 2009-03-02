
import os

cc_exts = ['.c', '.s']
linker_script = 'arch/x86/linker.ld'
arch_dir = 'arch/x86'
kern_dir = 'kernel'
lib_dir = 'lib'
loader_asm = 'loader.s'
output_binary = os.path.join(arch_dir, 'boot', 'kernel')

arch_sources = [os.path.join(arch_dir, f) for f in os.listdir(arch_dir) \
        if os.path.splitext(f)[1] in cc_exts and f != loader_asm]
kern_sources = [os.path.join(kern_dir, f) for f in os.listdir(kern_dir) \
        if os.path.splitext(f)[1] in cc_exts]
lib_sources = [os.path.join(lib_dir, f) for f in os.listdir(lib_dir) \
        if os.path.splitext(f)[1] in cc_exts]

env = Environment(
    AS = 'nasm', 
    CC = 'gcc', 
    CFLAGS = '-m32 -g -Wall -Wextra -isystem include', 
    LINKFLAGS = '-m32 -nostdlib -nostartfiles -nodefaultlibs -T %s' % 
            linker_script,
    ASFLAGS = '-f elf',
)

env.Decider('timestamp-match')
env.Program(output_binary, [os.path.join(arch_dir, loader_asm)] + 
        arch_sources + kern_sources + lib_sources)

