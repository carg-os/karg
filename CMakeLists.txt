cmake_minimum_required(VERSION 3.29)
project(karg C ASM)

if (NOT PLATFORM)
    set (PLATFORM VIRT)
endif()

add_library(karg
    src/dev.c
#    src/drivers/fb.c
#    src/drivers/fbcon.c
    src/drivers/tty.c
    src/drivers/uart.c
#    src/drivers/virtio-gpu.c
    src/entry.S
    src/fd.c
    src/init.c
    src/kmain.c
    src/kprintf.c
    src/mm/kalloc.c
    src/mm/page_alloc.c
    src/plic.c
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
#         src/platforms/virt/fb.c
#         src/platforms/virt/font.c
         src/platforms/virt/platform.c
#         src/platforms/virt/tty.c
     )
    set(PLATFORM_INCLUDE include/platforms/virt)
elseif (PLATFORM STREQUAL CV1800B)
     target_sources(karg PRIVATE
         src/platforms/cv1800b/platform.c
#         src/platforms/cv1800b/tty.c
     )
     set(PLATFORM_INCLUDE include/platforms/cv1800b)
elseif (PLATFORM STREQUAL D1)
     target_sources(karg PRIVATE
#         src/platforms/d1/ccu.c
#         src/platforms/d1/de.c
#         src/platforms/d1/fb.c
#         src/platforms/d1/font.c
#         src/platforms/d1/hdmi.c
         src/platforms/d1/platform.c
#         src/platforms/d1/tty.c
     )
    set(PLATFORM_INCLUDE include/platforms/d1)
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
target_compile_options(karg PRIVATE
    -ffreestanding
    -mcmodel=medany
    -Wall
    -Wextra
)
target_include_directories(karg PRIVATE include ${PLATFORM_INCLUDE})
target_compile_definitions(karg PRIVATE PLATFORM_${PLATFORM})
