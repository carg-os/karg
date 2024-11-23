# Karg
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

<img src="dark_logo.svg" width="128" height="128"> <img src="logo.svg" width="128" height="128"> <img src="bright_logo.svg" width="128" height="128">

Karg is an educational monolithic kernel designed to be the main component of [CargOS](https://github.com/carg-os/carg-os). It currently supports 64-bit RISC-V platforms like Milk-V Duo with a POSIX-compliant system call interface. Several crucial features are implemented elegantly and efficiently in the kernel, including: preemptive scheduling with priority, Unix-like process management, virtual memory, modular device drivers, interrupt handling subsystem and tiered memory allocator. Moreover, it comes with the capability to load ELF and Devicetree file formats, and is provided with the driver of common hardware interfaces such as RISC-V SBI and VirtIO-GPU.

## Features
* POSIX system calls

* Unix-like process management

* Preemptive scheduler with priority

* Time management

* Tiered memory allocator

* Virtual memory

* Modular device drivers

  * TTY subsystem and line discipline

  * VirtIO and VirtIO-GPU

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
