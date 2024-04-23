# Karg
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

<img src="dark_logo.svg" width="128" height="128"> <img src="logo.svg" width="128" height="128"> <img src="bright_logo.svg" width="128" height="128">

Karg serves as the kernel of CargOS, an educational Unix-like operating system specifically designed for developers. The system is meticulously crafted to balance comprehensive functionality with simplicity. Targeting the expansive community of developers, CargOS aims to establish a development environment from scratch, supporting its own evolution while maintaining efficiency for deployment on low-cost embedded systems. Engaging with and contributing to the development of CargOS offers substantial benefits for those looking to enrich their development experience!

## Build and Emulate
CargOS utilizes C23, the latest revision of C. However, it's not currently compatible with the RISC-V GNU toolchains provided by most major Linux distributions. Consequently, users have the option to either compile it from the source or use our pre-compiled binaries. Additional details can be found at [riscv-gnu-toolchain](https://github.com/carg-os/riscv-gnu-toolchain).

To build CargOS, execute the following commands. Keep in mind that the `CROSS_PREFIX` variable in the third instruction specifies the path to the RISC-V GNU toolchain. Adjust it accordingly if your toolchain has a different prefix.
```shell
git clone https://github.com/carg-os/karg.git
cd karg
make CROSS_PREFIX=riscv64-unknown-elf-
```

You can emulate CargOS using [QEMU](https://www.qemu.org/), which can be done using the following instruction.
```shell
make run
```