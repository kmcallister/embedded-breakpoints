# Usage:
# $ gdb -x gdb-with-breakpoints.py
# (gdb) file PROGRAM
# (gdb) break-embed PROGRAM

# An additional gdb wrapper like gdb-with-breakpoints.c
# that issues a "break-embed" if a program is specified on the command line
# would make using this more pleasant.

import struct

import elftools.elf.elffile

def _elf_file_embedded_breakpoints(filename):
    f = elftools.elf.elffile.ELFFile(open(filename))
    s = f.get_section_by_name('embed-breakpoints')
    addrs = []
    if not s:
        return addrs

    if f.little_endian:
        endian_fmt = '<'
    else:
        endian_fmt = '>'
    if f.elfclass == 64:
        (addr_fmt, addr_size) = ('Q', 8)
    elif f.elfclass == 32:
        (addr_fmt, addr_size) = ('I', 4)
    else:
        raise ValueError('unsupported ELF class: "%s"' % (f.elfclass,))
    
    fmt = endian_fmt + addr_fmt
    d = s.data()
    for i in xrange(0, len(d), addr_size):
        addrs.append(struct.unpack_from(fmt, d, i)[0])
    return addrs

class _BreakEmbedded(gdb.Command):
    'Set breakpoints from the "embed-breakpoints" section of a file'
    def __init__(self):
        gdb.Command.__init__(self, 'break-embedded', gdb.COMMAND_BREAKPOINTS, gdb.COMPLETE_FILENAME)

    def invoke(self, arg, from_tty):
        filename = arg
        addrs = _elf_file_embedded_breakpoints(filename)
        if not addrs:
            raise gdb.GdbError('no breakpoints embedded in "%s"' % (filename,))
        for addr in addrs:
            gdb.Breakpoint('*%s' % (addr,))

_BreakEmbedded()
