#ifndef CCU_H__
#define CCU_H__

/*
 * Functions for interacting with the D1 clocks.
 *
 * Author: Julie Zelenski <zelenski@cs.stanford.edu>
 */

#include <types.h>

// only list the clocks that we are using

#define CCU_PLL_VIDEO0_CTRL_REG 0x0040 /* PLL_VIDEO0 Control Register */

#define CCU_DE_CLK_REG 0x0600 /* DE Clock Register */
#define CCU_DE_BGR_REG 0x060C /* DE Bus Gating Reset Register */

#define CCU_DPSS_TOP_BGR_REG 0x0ABC /* DPSS_TOP Bus Gating Reset Register */

#define CCU_HDMI_24M_CLK_REG 0x0B04 /* HDMI Clock Register */
#define CCU_HDMI_BGR_REG 0x0B1C     /* HMDI Bus Gating Reset Register */

#define CCU_TCONTV_CLK_REG 0x0B80 /* TCONTV Clock Register */
#define CCU_TCONTV_BGR_REG 0x0B9C /* TCONTV Bus Gating Reset Register */

#define CCU_HSTIMER_BGR_REG 0x073C /* HSTimer Bus Gating Reset Register */

#define CCU_UART_BGR_REG 0x090C /* UART Bus Gating Reset Register */

void ccu_enable_pll(u32 reg, u32 M, u32 N);

void ccu_enable_bus_clk(u32 reg, u32 gating, u32 reset);

void ccu_write(u32 reg, u32 val);

u32 ccu_read(u32 reg);

u64 ccu_ahb0_frequency(void);

#endif
