#pragma once

#if __SIZEOF_POINTER__ > 4
#define ASM_PTR ".quad"
#else
#define ASM_PTR ".long"
#endif

#define EMBED_BREAKPOINT \
    asm("0:"                              \
        ".pushsection embed-breakpoints;" \
        ASM_PTR " 0b;"                    \
        ".popsection;")
