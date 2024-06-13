#include <config.h>

ENTRY(_start)
SECTIONS {
    . = BOOT_ENTRY_BASE;

    .init : {
        *(.init)
    }

    . = ALIGN(4K);
    PROVIDE(_pre_init_start = .);
    .init.pre_init : {
        *(.init.pre_init)
    }
    PROVIDE(_pre_init_end = .);

    PROVIDE(_init_start = .);
    .init.init : {
        *(.init.init)
    }
    PROVIDE(_init_end = .);

    PROVIDE(_post_init_start = .);
    .init.post_init : {
        *(.init.post_init)
    }
    PROVIDE(_post_init_end = .);

    PROVIDE(_dev_init_start = .);
    .init.dev : {
        *(.init.dev)
    }
    PROVIDE(_dev_init_end = .);

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
