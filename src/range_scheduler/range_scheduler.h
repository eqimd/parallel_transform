#pragma once

#include <stddef.h>
#include <stdint.h>
#include <utility>

using position = size_t;
using length = size_t;


class range_scheduler {
public:
    range_scheduler(size_t orig_sz, size_t subrange_len);
    ~range_scheduler();
    std::pair<position, length> get_subrange();
private:
    void* _orig_sz_ptr;
    void* _cur_pos_ptr;
    void* _subrange_len_ptr;

    size_t* _orig_sz;
    size_t* _cur_pos;
    size_t* _subrange_len;

    void* sched_futex_ptr;
    int32_t* sched_futex;
};