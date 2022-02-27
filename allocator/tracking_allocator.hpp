#ifndef _TRACKING_ALLOCATOR_HEADER_HPP_
#define _TRACKING_ALLOCATOR_HEADER_HPP_ 1
#pragma once

#include <memory_resource>
#include <iostream>
#include <iomanip>
#include <string>

#include "profiler.hpp"

class TrackingAllocator : public std::pmr::memory_resource
{
public:
    TrackingAllocator(std::pmr::memory_resource* baseAllocator = std::pmr::get_default_resource(),
                      std::string prefix = {}, bool inlinePrint = false)
            : baseAllocator_(baseAllocator), prefix_(std::move(prefix)), inlinePrint_(inlinePrint)
    {
        if (!baseAllocator_) throw std::runtime_error("Base Allocator is an empty pointer");
    }

    std::size_t poll() const
    {
        std::cout << std::setw(prefix_.size() + 1) << prefix_ << " -- "
                  << "Number of bytes allocated until now: " << numOfBytesAllocated_ << std::endl;
        return numOfBytesAllocated_;
    }

private:
    void* do_allocate(std::size_t numBytes, std::size_t alignment) override
    {
        if (inlinePrint_) {
            std::cout << std::setw(prefix_.size() + 1) << prefix_ << " -- [Allocating...]" << std::setw(45)
                      << "Number of bytes allocated in this call: " << std::setw(3) << numBytes
                      << " \tNumber of bytes allocated until now: " << numOfBytesAllocated_ << std::endl;
        }
        numOfBytesAllocated_ += numBytes;
        return baseAllocator_->allocate(numBytes, alignment);
    }

    void do_deallocate(void* p, std::size_t numBytes, std::size_t alignment) override
    {
        if (inlinePrint_) {
            // std::cout << std::setw(prefix_.size() + 1) << prefix_ << " -- Deallocating ..." << std::endl;

            // std::cout << std::setw(prefix_.size() + 1) << prefix_ << " -- ";
            // for (std::size_t i = 0; i < numBytes; ++i) std::cout << *(static_cast<char*>(p) + i);
            // std::cout << "'\n";

            std::cout << std::setw(prefix_.size() + 1) << prefix_ << " -- [Deallocating...]" << std::setw(45)
                      << "Number of bytes deallocated in this call: " << std::setw(3) << numBytes
                      << " \tNumber of bytes allocated until now: " << numOfBytesAllocated_ << std::endl;
        }
        numOfBytesAllocated_ -= numBytes;
        baseAllocator_->deallocate(p, numBytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        if (this == &other) return true;

        const TrackingAllocator* p = dynamic_cast<const TrackingAllocator*>(&other);
        if (p && p->baseAllocator_->is_equal(*this->baseAllocator_) &&
            p->numOfBytesAllocated_ == this->numOfBytesAllocated_)
            return true;

        return false;
    }

private:
    std::pmr::memory_resource* baseAllocator_{};
    std::size_t numOfBytesAllocated_ = 0;
    std::string prefix_{};
    bool inlinePrint_{false};
};

#endif