cmake_minimum_required(VERSION 3.29)
project(karg C ASM)

if (NOT PLATFORM)
    set (PLATFORM VIRT)
endif()

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
    src/tty.c
    src/uart.c
)
if (PLATFORM STREQUAL VIRT)
    target_sources(karg PRIVATE
        src/platforms/virt/platform.c
        src/platforms/virt/tty.c
    )
elseif (PLATFORM STREQUAL CV1800B)
    target_sources(karg PRIVATE
        src/platforms/cv1800b/platform.c
        src/platforms/cv1800b/tty.c
    )
elseif (PLATFORM STREQUAL D1)
    target_sources(karg PRIVATE
        src/platforms/d1/ccu.c
        src/platforms/d1/de.c
        src/platforms/d1/fb.c
        src/platforms/d1/font.c
        src/platforms/d1/hdmi.c
        src/platforms/d1/platform.c
        src/platforms/d1/tty.c
    )
endif()

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

if (PLATFORM STREQUAL VIRT)
    target_include_directories(karg PRIVATE include/platforms/virt)
elseif (PLATFORM STREQUAL CV1800B)
    target_include_directories(karg PRIVATE include/platforms/cv1800b)
elseif (PLATFORM STREQUAL D1)
    target_include_directories(karg PRIVATE include/platforms/d1)
endif()
