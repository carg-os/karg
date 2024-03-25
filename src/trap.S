    .section .text

    .global trap_entry
    .align 2
trap_entry:
    csrrw sp, sscratch, sp
    add sp, sp, -176

    sd a0, 112(sp)
    sd a1, 120(sp)
    sd a2, 128(sp)
    sd a3, 136(sp)
    sd a4, 144(sp)
    sd a5, 152(sp)
    sd a6, 160(sp)
    sd a7, 168(sp)

    csrr a0, sstatus
    csrr a1, sepc
    csrr a2, scause
    csrr a3, stval
    sd a0, 80(sp)
    sd a1, 88(sp)
    sd a2, 96(sp)
    sd a3, 104(sp)

    csrr a0, sscratch
    sd a0, 72(sp)
    csrw sscratch, sp

    sd ra, 64(sp)
    sd t0, 56(sp)
    sd t1, 48(sp)
    sd t2, 40(sp)
    sd t3, 32(sp)
    sd t4, 24(sp)
    sd t5, 16(sp)
    sd t6, 8(sp)

    mv a0, sp
    add a0, a0, 80
    jal trap_handler

    ld t6, 8(sp)
    ld t5, 16(sp)
    ld t4, 24(sp)
    ld t3, 32(sp)
    ld t2, 40(sp)
    ld t1, 48(sp)
    ld t0, 56(sp)
    ld ra, 64(sp)

    ld a0, 72(sp)
    csrw sscratch, a0

    ld a0, 80(sp)
    ld a1, 88(sp)
    csrw sstatus, a0
    csrw sepc, a1

    ld a0, 112(sp)
    ld a1, 120(sp)
    ld a2, 128(sp)
    ld a3, 136(sp)
    ld a4, 144(sp)
    ld a5, 152(sp)
    ld a6, 160(sp)
    ld a7, 168(sp)

    add sp, sp, 176
    csrrw sp, sscratch, sp
    sret
