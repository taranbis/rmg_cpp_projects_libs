#ifndef _SEGREGATOR_ALLOCATOR_HEADER_HPP_
#define _SEGREGATOR_ALLOCATOR_HEADER_HPP_ 1
#pragma once

#include <memory_resource>
#include <iostream>
#include <iomanip>
#include <string>

class Segregator : public std::pmr::memory_resource
{
public:
    Segregator(std::pmr::memory_resource* smallAllocator, std::pmr::memory_resource* largeAllocator,
               std::size_t threshold)
            : smallAllocator_(smallAllocator), largeAllocator_(largeAllocator), threshold_(threshold)
    {
    }

private:
    void* do_allocate(std::size_t numBytes, std::size_t alignment) override
    {
        if (numBytes <= threshold_) { return smallAllocator_->allocate(numBytes, alignment); }
        return largeAllocator_->allocate(numBytes, alignment);
    }

    void do_deallocate(void* p, std::size_t numBytes, std::size_t alignment) override
    {
        if (!p) {
            return;
        }

        if (numBytes <= threshold_) {
            smallAllocator_->deallocate(p, numBytes, alignment);
            return;
        }
        largeAllocator_->deallocate(p, numBytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        if (this == &other) return true;

        const Segregator* seg = dynamic_cast<const Segregator*>(&other);
        if (seg && seg->smallAllocator_->is_equal(*this->smallAllocator_) &&
            seg->largeAllocator_->is_equal(*this->largeAllocator_) && seg->threshold_ == this->threshold_)
            return true;

        return false;
    }

private:
    std::pmr::memory_resource* smallAllocator_{};
    std::pmr::memory_resource* largeAllocator_{};
    std::size_t threshold_{};
};

#endif