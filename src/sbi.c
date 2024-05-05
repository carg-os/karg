#include <errno.h>
#include <types.h>

typedef struct {
    i32 err;
    isize val;
} sbi_res_t;

sbi_res_t sbi_ecall(i32 eid, i32 fid, usize arg0, usize arg1, usize arg2,
                    usize arg3, usize arg4, usize arg5) {
    register size_t a0 asm("a0") = arg0;
    register size_t a1 asm("a1") = arg1;
    register size_t a2 asm("a2") = arg2;
    register size_t a3 asm("a3") = arg3;
    register size_t a4 asm("a4") = arg4;
    register size_t a5 asm("a5") = arg5;
    register size_t a6 asm("a6") = fid;
    register size_t a7 asm("a7") = eid;
    asm volatile("ecall"
                 : "+r"(a0), "+r"(a1)
                 : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7));
    return (sbi_res_t){
        .err = a0,
        .val = a1,
    };
}

#define SBI_SUCCESS 0
#define SBI_ERR_FAILED -1
#define SBI_ERR_NOT_SUPPORTED -2
#define SBI_ERR_INVALID_PARAM -3
#define SBI_ERR_DENIED -4
#define SBI_ERR_INVALID_ADDRESS -5
#define SBI_ERR_ALREADY_AVAILABLE -6
#define SBI_ERR_ALREADY_STARTED -7
#define SBI_ERR_ALREADY_STOPPED -8

i32 sbi_map_err(i32 err) {
    switch (err) {
    case SBI_SUCCESS:
        return 0;
    case SBI_ERR_FAILED:
        return -EPERM;
    case SBI_ERR_NOT_SUPPORTED:
        return -ENOTSUP;
    case SBI_ERR_INVALID_PARAM:
        return -EINVAL;
    case SBI_ERR_DENIED:
        return -EPERM;
    case SBI_ERR_INVALID_ADDRESS:
        return -EFAULT;
    case SBI_ERR_ALREADY_AVAILABLE:
    case SBI_ERR_ALREADY_STARTED:
    case SBI_ERR_ALREADY_STOPPED:
        return -EPERM;
    default:
        return -ENOTSUP;
    };
}
