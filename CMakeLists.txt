project(karg C ASM)

add_library(karg
    src/dev.c
    src/driver.c
    src/entry.S
    src/fd.c
    src/kalloc.c
    src/kmain.c
    src/kprintf.c
    src/page_alloc.c
    src/plic.c
    src/proc.c
    src/proc.S
    src/sbi.c
    src/sched.c
    src/sem.c
    src/syscall.c
    src/timer.c
    src/trap.c
    src/trap.S
    src/uart.c
)

set_property(TARGET karg PROPERTY C_STANDARD 23)
set_property(TARGET karg PROPERTY COMPILE_WARNING_AS_ERROR ON)
target_compile_options(karg PRIVATE
    -ffreestanding
    -mcmodel=medany
    -Wall
    -Wextra
)
target_include_directories(karg PRIVATE include)
target_compile_definitions(karg PRIVATE PLATFORM_${PLATFORM})
