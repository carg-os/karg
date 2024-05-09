#if defined(PLATFORM_LICHEE_RV_DOCK)
#include <platforms/lichee_rv_dock.h>
#elif defined(PLATFORM_MILKV_DUO)
#include <platforms/milkv_duo.h>
#elif defined(PLATFORM_VIRT)
#include <platforms/virt.h>
#else
#error Unknown platform specified
#endif
