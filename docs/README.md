# Karg
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

Karg is the kernel of [CargOS](https://github.com/carg-os/carg-os). It abstracts the low-level interfaces of devices and implements the core functionalities of the system. Essential features such as process management, I/O management and file system are all provided by it.

## FAQ
### Why not Rust?
We tried, initially, but encountered numerous quirks and inconveniences. For instance, it lacks support for essential low-level structures like anonymous unions, contains numerous nightly features, and adheres to conventions that aren't appropriate for a single-core kernel. Additionally, the requirements of its standard traits are way too strict. Despite these issues, Rust remains a viable option for user space programs.
