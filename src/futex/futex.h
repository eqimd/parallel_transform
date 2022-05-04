#pragma once

#include <stdint.h>


static int
futex(int32_t *uaddr, int32_t futex_op, int32_t val,
        const struct timespec *timeout, int32_t *uaddr2, int32_t val3);

static void
fwait(int32_t *futexp);

static void
fpost(int32_t *futexp);