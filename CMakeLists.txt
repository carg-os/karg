cmake_minimum_required(VERSION 3.29)
project(karg C ASM)

if (NOT PLATFORM)
    set (PLATFORM VIRT)
endif()

add_executable(karg
    src/dev.c
    src/drivers/ldisc.c
    src/drivers/plic.c
    src/drivers/tty.c
    src/drivers/uart.c
    src/entry.S
    src/init.c
    src/intr.c
    src/kmain.c
    src/log.c
    src/mm/kalloc.c
    src/mm/page_alloc.c
    src/proc.c
    src/proc.S
    src/sbi.c
    src/sched.c
    src/sem.c
    src/syscalls/exit.c
    src/syscalls/getpid.c
    src/syscalls/proc.c
    src/syscalls/read.c
    src/syscalls/reboot.c
    src/syscalls/sleep.c
    src/syscalls/syscall.c
    src/syscalls/wait.c
    src/syscalls/write.c
    src/syscalls/yield.c
    src/time.c
    src/timer.c
    src/trap.c
    src/trap.S
)

if (PLATFORM STREQUAL VIRT)
    target_sources(karg PRIVATE
        src/platforms/virt.c
    )
endif()

add_dependencies(karg linker-script)
add_custom_target(linker-script DEPENDS kernel.ld)
add_custom_command(OUTPUT kernel.ld
    COMMAND ${CMAKE_C_COMPILER} -E -x c
                -I ${PROJECT_SOURCE_DIR}/include
                ${PROJECT_SOURCE_DIR}/kernel.ld > kernel.ld
    MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/kernel.ld
)

set_property(TARGET karg PROPERTY C_STANDARD 23)
set_property(TARGET karg PROPERTY COMPILE_WARNING_AS_ERROR ON)
set_property(TARGET karg PROPERTY LINK_DEPENDS ${PROJECT_BINARY_DIR}/kernel.ld)
target_compile_options(karg PRIVATE
    -ffreestanding
    -mcmodel=medany
    -Wall
    -Wextra
)
target_link_options(karg PRIVATE -nostdlib -T kernel.ld)
target_include_directories(karg PRIVATE include)
