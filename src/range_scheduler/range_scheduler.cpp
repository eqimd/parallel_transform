#include "range_scheduler.h"
#include <stddef.h>
#include <sys/mman.h>
#include <algorithm>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <string.h>


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


range_scheduler::range_scheduler(size_t orig_sz, size_t subrange_len) {
    errno = 0;
    _orig_sz_ptr = mmap(
        NULL,
        sizeof(size_t),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );
    if (_orig_sz_ptr == MAP_FAILED) {
        throw std::runtime_error("Could not mmap; " + std::string(strerror(errno)));
    }

    errno = 0;
    _cur_pos_ptr = mmap(
        NULL,
        sizeof(size_t),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );
    if (_cur_pos_ptr == MAP_FAILED) {
        throw std::runtime_error("Could not mmap; " + std::string(strerror(errno)));
    }

    errno = 0;
    _subrange_len_ptr = mmap(
        NULL,
        sizeof(size_t),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );
    if (_subrange_len_ptr == MAP_FAILED) {
        throw std::runtime_error("Could not mmap; " + std::string(strerror(errno)));
    }

    errno = 0;
    sched_futex_ptr = mmap(
        NULL,
        sizeof(int32_t),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );
    if (sched_futex_ptr == MAP_FAILED) {
        throw std::runtime_error("Could not mmap; " + std::string(strerror(errno)));
    }

    _orig_sz = reinterpret_cast<size_t*>(_orig_sz_ptr);
    _subrange_len = reinterpret_cast<size_t*>(_subrange_len_ptr);
    _cur_pos = reinterpret_cast<size_t*>(_cur_pos_ptr);
    sched_futex = reinterpret_cast<int32_t*>(sched_futex_ptr);

    *_orig_sz = orig_sz;
    *_subrange_len = subrange_len;
    *_cur_pos = 0;
    *sched_futex = 1;   /* Make futex available */



}

range_scheduler::~range_scheduler() {
    munmap(sched_futex_ptr, sizeof(int32_t));
    munmap(_orig_sz_ptr, sizeof(size_t));
    munmap(_subrange_len_ptr, sizeof(size_t));
    munmap(_cur_pos_ptr, sizeof(size_t));
}

std::pair<position, length> range_scheduler::get_subrange() {
    fwait(sched_futex);

    if (_cur_pos >= _orig_sz) {
        fpost(sched_futex);
        return {*_orig_sz, 0};
    }

    size_t len = std::min(*_subrange_len, *_orig_sz - *_cur_pos);
    size_t ret_pos = *_cur_pos;
    (*_cur_pos) += len;

    fpost(sched_futex);

    return {ret_pos, len};
}