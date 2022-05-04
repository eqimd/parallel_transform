#include "read_scheduler.h"
#include <stddef.h>
#include "futex/futex.h"
#include <sys/mman.h>
#include <algorithm>


range_scheduler::range_scheduler(size_t orig_sz, size_t subrange_sz) {
    _orig_sz = orig_sz;
    _subrange_sz = subrange_sz;
    _cur_pos = 0;

    sched_futex = reinterpret_cast<int32_t*>(
        mmap(
            NULL,
            sizeof(int32_t),
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS,
            -1,
            0
        )
    );

    *sched_futex = 1;   /* Make futex available */
}

range_scheduler::~range_scheduler() {
    munmap(sched_futex, sizeof(int32_t));
}

std::pair<position, length> range_scheduler::get_subrange() {
    fwait(sched_futex);

    if (_cur_pos >= _orig_sz) {
        fpost(sched_futex);
        return {-1, -1};
    }

    size_t len = std::min(_subrange_sz, _orig_sz - _cur_pos);
    size_t ret_pos = _cur_pos;
    _cur_pos += len;

    fpost(sched_futex);

    return {ret_pos, len};
}