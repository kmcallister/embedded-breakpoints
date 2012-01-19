#pragma once

#if __SIZEOF_POINTER__ > 4
#define EMBED_BREAKPOINT_PTR ".quad"
#else
#define EMBED_BREAKPOINT_PTR ".long"
#endif

#define EMBED_BREAKPOINT \
    asm("0:"                              \
        ".pushsection embed-breakpoints;" \
        EMBED_BREAKPOINT_PTR " 0b;"       \
        ".popsection;")
