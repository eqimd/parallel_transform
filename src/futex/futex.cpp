#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <atomic>
#include <linux/futex.h>
#include <sys/time.h>
#include "futex.h"

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

static int
futex(int32_t *uaddr, int32_t futex_op, int32_t val,
        const struct timespec *timeout, int32_t *uaddr2, int32_t val3)
{
    return syscall(SYS_futex, uaddr, futex_op, val,
                    timeout, uaddr, val3);
}

static void
fwait(int32_t *futexp)
{
    int32_t s;

    while (1) {
        if (__sync_bool_compare_and_swap(futexp, 1, 0))
            break;

        s = futex(futexp, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (s == -1 && errno != EAGAIN)
            errExit("futex-FUTEX_WAIT");
    }
}

static void
fpost(int32_t *futexp)
{
    int32_t s;

    if (__sync_bool_compare_and_swap(futexp, 0, 1)) {

        s = futex(futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (s  == -1)
            errExit("futex-FUTEX_WAKE");
    }
}