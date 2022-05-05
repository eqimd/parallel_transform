#pragma once

#include "vector_shared.h"
#include "range_scheduler/range_scheduler.h"
#include <unistd.h>
#include <sys/wait.h>
#include <ranges>
#include <stdexcept>
#include <string.h>


template <typename R>
concept InputContiguousRange = std::ranges::contiguous_range<R>;

template <typename F, typename A>
concept StatelessFunction = std::invocable<F, A>;

template <
    typename R,
    typename F, 
    typename RV = std::ranges::range_value_t<R>,
    typename IR = std::invoke_result<F, RV>::type
>
    requires (InputContiguousRange<R>, StatelessFunction<F, RV>, std::is_trivially_copyable_v<IR>)
void transform_static(vector_shared<IR>& vec_trans, R& range, F f, size_t pos, size_t len) {
    for (size_t i = pos; i < pos + len; ++i) {
        vec_trans[i] = f(range.data()[i]);
    }
}

template <
    typename R,
    typename F, 
    typename RV = std::ranges::range_value_t<R>,
    typename IR = std::invoke_result<F, RV>::type
>
    requires (InputContiguousRange<R>, StatelessFunction<F, RV>, std::is_trivially_copyable_v<IR>)
void transform_dynamic_balancer(vector_shared<IR>& vec_trans, R& range, F f, range_scheduler& rsched) {
    std::pair<position, length> p = rsched.get_subrange();
    while (p.second != 0) {
        for (size_t i = p.first; i < p.first + p.second; ++i) {
            vec_trans[i] = f(range.data()[i]);
        }
        p = rsched.get_subrange();
    }
}

template <
    typename R,
    typename F, 
    typename RV = std::ranges::range_value_t<R>,
    typename IR = std::invoke_result<F, RV>::type
>
    requires (InputContiguousRange<R>, StatelessFunction<F, RV>, std::is_trivially_copyable_v<IR>)
std::vector<IR> TransformWithProcesses(R&& range, F&& f, size_t nprocesses, size_t subrange_len) {
    size_t range_size = range.size();
    range_scheduler rsched(range_size, subrange_len);

    vector_shared<IR> vec(range_size);
    std::vector<pid_t> childs;
    childs.reserve(range_size);

    size_t equal_subrange = (range_size + nprocesses - 1) / nprocesses;
    size_t len;
    if (subrange_len == 0) {
        len = equal_subrange;
    } else {
        len = subrange_len;
    }
    size_t cur_pos = 0;

    for (size_t i = 0; i < nprocesses && cur_pos < range_size; ++i) {
        len = std::min(len, range_size - cur_pos);
        errno = 0;
        pid_t pid = fork();

        if (pid == 0) {
            /* Enter child */

            childs.push_back(pid);

            if (subrange_len == 0) {
                transform_static(vec, range, f, cur_pos, len);
            } else {
                transform_dynamic_balancer(vec, range, f, rsched);
            }

            exit(EXIT_SUCCESS);
        }

        if (pid == -1) {
            /* fork failed */

            int fork_errno = errno;

            for (auto chld : childs) {
                kill(chld, SIGKILL);
            }

            throw std::runtime_error(
                "Could not fork process; " +
                std::string(strerror(fork_errno))
            );
        }
        cur_pos += len;
    }
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0) {}

    return std::vector(vec.begin(), vec.end());
}
