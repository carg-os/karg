# Karg
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

<img src="dark_logo.svg" width="128" height="128"> <img src="logo.svg" width="128" height="128"> <img src="bright_logo.svg" width="128" height="128">

Karg is an educational monolithic kernel designed to be the main component of [CargOS](https://github.com/carg-os/carg-os). It currently supports 64-bit RISC-V platforms like Milk-V Duo with a POSIX-compliant system call interface. Several crucial features are implemented elegantly and efficiently in the kernel, including: preemptive scheduling with priority, Unix-like process management, virtual memory, modular device drivers, interrupt handling subsystem and tiered memory allocator. Moreover, it comes with the capability to load ELF and Devicetree file formats, and is provided with the driver of common hardware interfaces such as RISC-V SBI and VirtIO-GPU.

## Features
* **POSIX system calls** \
  This kernel supports system calls based on the POSIX standard, organized into the following categories:
    * **Device I/O**: `open`, `close`, `read`, `write`, `seek`, `ioctl`
    * **Process Management**: `getpid`, `wait`, `exit`, `kill`
    * **Scheduling**: `sleep`, `yield`, `setpriority`
    * **Miscellaneous**: `posix_spawn`, `sbrk`, `reboot`

* **Unix-like process management** \
  The processes in this system are managed in a hierarchical tree structure, similar to that of Unix-like systems. Each process owns various resources, such as a page table, file descriptors, and timers, and maintains pointers to its parent process and two types of child processes: regular children and zombie children. Each process operates as a state machine and can exist in one of the following states:
  * **INIT**: Initialized but not yet passed to other components.
  * **READY**: Ready to be scheduled and run.
  * **CURR**: Currently executing, with the `curr_proc` variable pointing to it.
  * **WAIT**: Waiting for an event, such as a semaphore signal or the termination of a child process.
  * **SLEEP**: Sleeping, typically waiting for a specified duration.
  * **ZOMBIE**: Exited, with an error code available for the parent to retrieve.

  By default, certain PIDs are also reserved for specific system functions. PID 0 is allocated to the idle process, which simply waits when there are no other processes ready to be scheduled. PID 1 is assigned to the [init](https://github.com/carg-os/init) process, which serves as the root of the process tree and the entry point to the system's shell.

* **Preemptive scheduler with priority** \
  The current scheduler simply implements a round-robin strategy, combined with a priority queue that sorts processes based on their priority. When performing a context switch, the scheduler selects the highest-priority process from the ready queue (which contains only processes that are ready to run) and sets it as the currently executing process. The global timer is then updated and passed to the time management subsystem to wait for the next tick.

  For context switching, only callee-saved registers are backed up and restored, as the caller-saved registers are already saved during the entry to the interrupt handler.

* **Time management** \
  Whenever a waiting operation is triggered by either the scheduler's new time tick or the invocation of the `sleep` system call, the time management subsystem inserts a new timer entry into an ordered callback queue. Each timer entry is then sorted based on its specified time. When the timer expires, the corresponding callback is executed, and the system updates the time for the next clock interrupt. This mechanism ensures efficient and accurate handling of time-dependent operations, such as process sleep and event waiting.

* **Tiered memory allocator** \
  The memory allocator in the system is divided into two components: page_alloc and kmalloc, each serving a distinct purpose in memory management:

    * **page_alloc** \
      page_alloc manages the entire physical memory as a series of pages, each 4096 bytes in size. It is responsible for allocating and freeing these pages, which serve as the fundamental unit of memory management in low-level interactions with hardware mechanisms.

    * **kmalloc** \
      Built on top of page_alloc, kmalloc handles the allocation of smaller blocks of memory for kernel operations. It requests pages from page_alloc and then subdivides these pages into smaller, contiguous regions of memory of arbitrary sizes. The current implementation is based on the first-fit allocation policy.

  In summary, page_alloc manages physical memory pages, while kmalloc allocates smaller, contiguous memory blocks for kernel use.

* **Virtual memory** \
  The virtual memory subsystem employs the RISC-V Sv39 virtual memory mechanism. When a new process is created, the system parses the ELF executable header to set up the necessary memory mappings, allocate pages for data, and reserve space for the stack and dynamic memory. Before transitioning to user space, the kernel also configures its own address space, establishing direct mappings from virtual memory in kernel space to physical memory for regions accessed via MMIO. At the top of each virtual address space, a trampoline page serves as the shared entry point for the interrupt handler.

* **Modular device drivers** \
  The system is organized into units called "modules", with each subsystem and driver treated as a separate module. Each module automatically registers itself for initialization by placing symbols in the `.init` section. Modules are then initialized in three successive phases: pre-initialization, initialization, and post-initialization. This design ensures that dependencies between modules (for example, the scheduler can only be initialized after the memory allocator) are properly satisfied.

  The general device driver interface provides six functions: `open`, `close`, `read`, `write`, `seek`, and `ioctl`. Each driver manages its own devices, which are represented by `dev_t` in the file descriptors and identified by a minor device number. Additionally, drivers that do not directly correspond to physical hardware, but instead serve as abstraction layers, such as the TTY and line discipline subsystems, are also implemented.

  * **TTY subsystem and line discipline** \
    The TTY subsystem is responsible for managing various text-based hardware devices in the system. It provides a read-write abstraction that can be bound to an input source and multiple output sinks. The line discipline subsystem, on the other hand, processes and converts special characters (such as CRLF) and ANSI escape sequences before passing the requests to the underlying driver, with buffering applied.

  * **VirtIO and VirtIO-GPU** \
    The system automatically detects VirtIO-MMIO devices in the address space and invokes their corresponding drivers. It then allocates and creates a linked list of descriptors, which are passed to the VirtIO device's queue when a command needs to be sent. Additionally, VirtIO-GPU is supported for displaying content in the framebuffer, using tricks such as dynamically changing the framebuffer address. This enables functions like page scrolling to be performed without frequent memory copying.

## Usage
Karg is the kernel of the CargOS operating system, but it is not a complete OS on its own. To help you build and run the full CargOS system, we provide a separate repository that automatically clones and assembles all necessary components, including the kernel, the init process, the C standard library implementation and a Lua port. For more details, please visit [CargOS](https://github.com/carg-os/carg-os).
```sh
git clone https://github.com/carg-os/carg-os.git && cd carg-os
make
```

To contribute code to the Karg kernel, please adhere to the project's coding style. Before committing changes, ensure your code is properly formatted by running the following command:
```sh
make fmt
```

## FAQ
### Why not Rust?
We tried, initially, but encountered numerous quirks and inconveniences. For instance, it lacks support for essential low-level structures like anonymous unions, contains numerous nightly features, and adheres to conventions that aren't appropriate for a single-core kernel. Additionally, the requirements of its standard traits are way too strict. Despite these issues, Rust remains a viable option for user space programs.
