#pragma once

#include <vector>
#include <sys/mman.h>
#include <cmath>
#include <iostream>

/*
 * Remade code from
 * https://en.cppreference.com/w/cpp/named_req/Allocator
 */

template <class T>
struct shared_allocator
{
    typedef T value_type;

    shared_allocator() = default;
    template <class U>
    constexpr shared_allocator(const shared_allocator<U> &) noexcept {}

    [[nodiscard]] T *allocate(std::size_t n)
    {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_array_new_length();

        void *ptr = mmap(
            NULL,
            sizeof(T) * n,
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS,
            -1,
            0);
        T *ptr_casted = reinterpret_cast<T *>(ptr);
        if (ptr == MAP_FAILED)
        {
            throw std::bad_alloc();
        }

        return ptr_casted;
    }

    void deallocate(T *p, std::size_t n) noexcept
    {
        munmap(reinterpret_cast<void *>(p), n);
    }

private:
    void report(T *p, std::size_t n, bool alloc = true) const
    {
        std::cout << (alloc ? "mmap: " : "munmap : ") << sizeof(T) * n
                  << " bytes at " << std::hex << std::showbase
                  << reinterpret_cast<void *>(p) << std::dec << '\n';
    }
};

template <typename T>
using vector_shared = std::vector<T, shared_allocator<T>>;
