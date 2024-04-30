#if defined(PLATFORM_QEMU)
#include <platforms/qemu.h>
#elif defined(PLATFORM_MILKV_DUO)
#include <platforms/milkv_duo.h>
#else
#error Unknown platform specified
#endif
