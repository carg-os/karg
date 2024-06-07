cmake_minimum_required(VERSION 3.29)
project(karg C ASM)

if (NOT PLATFORM)
    set (PLATFORM VIRT)
endif()

add_executable(karg
    src/dev.c
    src/dev_table.c
    src/drivers/fb.c
    src/drivers/fbcon.c
    src/drivers/font.c
    src/drivers/plic.c
    src/drivers/tty.c
    src/drivers/uart.c
    src/drivers/virtio-gpu.c
    src/entry.S
    src/fd.c
    src/init.c
    src/kmain.c
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
    src/syscalls/sleepns.c
    src/syscalls/syscall.c
    src/syscalls/wait.c
    src/syscalls/write.c
    src/syscalls/yield.c
    src/timer.c
    src/trap.c
    src/trap.S
)

if (PLATFORM STREQUAL VIRT)
    target_sources(karg PRIVATE
        src/platforms/virt.c
    )
    set(PLATFORM_INCLUDE include/platforms/virt)
endif()

add_dependencies(karg linker-script)
add_custom_target(linker-script DEPENDS kernel.ld)
add_custom_command(OUTPUT kernel.ld
    COMMAND ${CMAKE_C_COMPILER} -E -x c
                -I ${PROJECT_SOURCE_DIR}/${PLATFORM_INCLUDE}
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
target_include_directories(karg PRIVATE include ${PLATFORM_INCLUDE})
target_compile_definitions(karg PRIVATE PLATFORM_${PLATFORM})
