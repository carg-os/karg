#include <boot.h>

ENTRY(_start)
SECTIONS {
    . = BOOT_ENTRY_BASE;

    .init : {
        *(.init .init.*)
    }

    .text : {
        *(.text .text.*)
    }

    . = ALIGN(4K);
    .rodata : {
        *(.rodata .rodata.*)
    }

    . = ALIGN(4K);
    .data : {
        *(.data .data.*)
    }

    .sdata : {
        *(.sdata .sdata.*)
    }

    . = ALIGN(8);
    PROVIDE(_bss_start = .);

    .sbss : {
        *(.sbss .sbss.*)
    }

    .bss : {
        *(.bss .bss.*)
    }

    . = ALIGN(8);
    PROVIDE(_bss_end = .);

    . = ALIGN(4K);
    PROVIDE(_heap_start = .);
}
