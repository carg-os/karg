#include <ccu.h>

#define CCU_BASE 0x2001000L

#define CCU(offset) *(volatile u32 *) (CCU_BASE + offset)

void ccu_write(u32 reg, u32 val) { CCU(reg) = val; }

u32 ccu_read(u32 reg) { return CCU(reg); }

void ccu_enable_pll(u32 reg, u32 M, u32 N) {
    CCU(reg) |= (1 << 31);                   // PLL_en
    CCU(reg) &= ~(1 << 27);                  // disable output while changing
    CCU(reg) &= ~((0xff << 8) | (0x1 << 1)); // clear M & N
    CCU(reg) |= (M << 8) | (N << 1);
    CCU(reg) |= (1 << 29); // lock enable
    // while (!(CCU(reg) & (1 << 28)))
    //     ;                // wait til stable
    CCU(reg) |= 1 << 27; // enable output
}

/*
 * From docs:
    For the Bus Gating Reset register of a module, the reset bit is de-asserted
 first, and then the clock gating bit is enabled to avoid potential problems
    caused by the asynchronous release of the reset signal.
 */
void ccu_enable_bus_clk(u32 reg, u32 gating, u32 reset) {
    CCU(reg) |= reset;  // de-assert reset
    CCU(reg) |= gating; // set gating to enable
}

u64 ccu_ahb0_frequency(void) { return 200 * 1000 * 1000; }
