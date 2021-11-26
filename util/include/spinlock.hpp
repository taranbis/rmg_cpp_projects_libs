#ifndef _SPINLOCK_HEADER_HPP_
#define _SPINLOCK_HEADER_HPP_ 1
#pragma once

/*
 * DRace, a dynamic data race detector
 *
 * Copyright 2018 Siemens AG
 *
 * Authors:
 *   Felix Moessbauer <felix.moessbauer@siemens.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <atomic>
#include <mutex>
#include <thread>

#ifdef HAVE_SSE2
#include <immintrin.h> //_mm_pause
#endif

namespace rmg
{
class spinlock
{
private:
    std::atomic<bool> locked_{false};

public:
    inline void lock() noexcept
    {
        for (int spinCount = 0; !try_lock(); ++spinCount) {
            if (spinCount < 16) {
#ifdef HAVE_SSE2
                // tell the CPU (not OS), that we are spin-waiting
                _mm_pause();
#endif
            } else {
                // avoid waisting CPU time in rare long-wait scenarios
                std::this_thread::yield();
            }
        }
    }

    inline bool try_lock() noexcept
    {
        return !locked_.exchange(true, std::memory_order_acquire);
    }

    inline void unlock() noexcept
    {
        locked_.store(false, std::memory_order_release);
    }
};

} // namespace rmg

#endif