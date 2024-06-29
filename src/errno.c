#include <errno.h>

const char *errno_name(i32 errno) {
#define CASE(errno)                                                            \
    case errno:                                                                \
        return #errno
    switch (errno) {
        CASE(E2BIG);
        CASE(EACCES);
        CASE(EADDRINUSE);
        CASE(EADDRNOTAVAIL);
        CASE(EAFNOSUPPORT);
        CASE(EAGAIN);
        CASE(EALREADY);
        CASE(EBADF);
        CASE(EBADMSG);
        CASE(EBUSY);
        CASE(ECANCELED);
        CASE(ECHILD);
        CASE(ECONNABORTED);
        CASE(ECONNREFUSED);
        CASE(ECONNRESET);
        CASE(EDEADLK);
        CASE(EDESTADDRREQ);
        CASE(EDOM);
        CASE(EDQUOT);
        CASE(EEXIST);
        CASE(EFAULT);
        CASE(EFBIG);
        CASE(EHOSTUNREACH);
        CASE(EIDRM);
        CASE(EILSEQ);
        CASE(EINPROGRESS);
        CASE(EINTR);
        CASE(EINVAL);
        CASE(EIO);
        CASE(EISCONN);
        CASE(EISDIR);
        CASE(ELOOP);
        CASE(EMFILE);
        CASE(EMLINK);
        CASE(EMSGSIZE);
        CASE(EMULTIHOP);
        CASE(ENAMETOOLONG);
        CASE(ENETDOWN);
        CASE(ENETRESET);
        CASE(ENETUNREACH);
        CASE(ENFILE);
        CASE(ENOBUFS);
        CASE(ENODATA);
        CASE(ENODEV);
        CASE(ENOENT);
        CASE(ENOEXEC);
        CASE(ENOLCK);
        CASE(ENOLINK);
        CASE(ENOMEM);
        CASE(ENOMSG);
        CASE(ENOPROTOOPT);
        CASE(ENOSPC);
        CASE(ENOSR);
        CASE(ENOSTR);
        CASE(ENOSYS);
        CASE(ENOTCONN);
        CASE(ENOTDIR);
        CASE(ENOTEMPTY);
        CASE(ENOTRECOVERABLE);
        CASE(ENOTSOCK);
        CASE(ENOTSUP);
        CASE(ENOTTY);
        CASE(ENXIO);
        CASE(EOPNOTSUPP);
        CASE(EOVERFLOW);
        CASE(EOWNERDEAD);
        CASE(EPERM);
        CASE(EPIPE);
        CASE(EPROTO);
        CASE(EPROTONOSUPPORT);
        CASE(EPROTOTYPE);
        CASE(ERANGE);
        CASE(EROFS);
        CASE(ESPIPE);
        CASE(ESRCH);
        CASE(ESTALE);
        CASE(ETIME);
        CASE(ETIMEDOUT);
        CASE(ETXTBSY);
        CASE(EXDEV);
    }
#undef CASE
    return nullptr;
}
