#pragma once

#define csr_read(reg)                                                          \
    ({                                                                         \
        usize val;                                                             \
        asm volatile("csrr %0, " #reg : "=r"(val));                            \
        val;                                                                   \
    })
#define csr_write(reg, val) asm volatile("csrw " #reg ", %0" : : "r"(val))
#define csr_set_bits(reg, bits) asm volatile("csrs " #reg ", %0" : : "r"(bits))
#define csr_clear_bits(reg, bits)                                              \
    asm volatile("csrc " #reg ", %0" : : "r"(bits))

#define CSR_SSTATUS_SPIE 0x20
#define CSR_SSTATUS_SPP 0x100

#define CSR_SIE_STIE 0x20
#define CSR_SIE_SEIE 0x200

#define CSR_SCAUSE_INTR 0x8000000000000000
#define CSR_SCAUSE_TIMER_INTR 5
#define CSR_SCAUSE_EXT_INTR 9
#define CSR_SCAUSE_ECALL_U 8
