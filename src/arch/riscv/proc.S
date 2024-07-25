    .section .text

    .global ctx_sw
ctx_sw:
    add sp, sp, -128
    sd ra, 120(sp)
    sd gp, 112(sp)
    sd tp, 104(sp)
    sd s0, 96(sp)
    sd s1, 88(sp)
    sd s2, 80(sp)
    sd s3, 72(sp)
    sd s4, 64(sp)
    sd s5, 56(sp)
    sd s6, 48(sp)
    sd s7, 40(sp)
    sd s8, 32(sp)
    sd s9, 24(sp)
    sd s10, 16(sp)
    sd s11, 8(sp)

    sd sp, 0(a0)
    ld sp, 0(a1)

    ld s11, 8(sp)
    ld s10, 16(sp)
    ld s9, 24(sp)
    ld s8, 32(sp)
    ld s7, 40(sp)
    ld s6, 48(sp)
    ld s5, 56(sp)
    ld s4, 64(sp)
    ld s3, 72(sp)
    ld s2, 80(sp)
    ld s1, 88(sp)
    ld s0, 96(sp)
    ld tp, 104(sp)
    ld gp, 112(sp)
    ld ra, 120(sp)
    add sp, sp, 128

    ret
