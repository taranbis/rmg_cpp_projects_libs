#ifndef _MEMORY_POOL_HEADER_HPP_
#define _MEMORY_POOL_HEADER_HPP_ 1
#pragma once

#include <memory_resource>

#include "util.hpp"

/****************************************************************************************************************
 * @brief Header file that defines the memory pool. By the benchmarks in benchmark.cpp
 * std::pmr::(un)unsynchronized_pool_resource seem really slow, so I wrote my own pool allocator. Way easier with
 * new pmr allocators. I don't know why it is slower than std version, but it is faster than the pmr version
 ****************************************************************************************************************/

struct Chunk {
    Chunk* next; // pointer to the next Chunk, when chunk is free
};

class MemoryPool : public std::pmr::memory_resource
{
private:
    Chunk* freePointer_{nullptr}; // pointer to the first free address
    size_t numChunks_{0};         // number of chunks in a block
    size_t chunkSize_{0};         // chunk size
    size_t blockSize_{0};         // block size

    size_t chunksAllocated_{0}; // will hold how much memory was allocated until now
    size_t chunksFree_{0};
public:
    MemoryPool(std::pmr::memory_resource* upstream, size_t chunkSize, size_t numChunks)
            : chunkSize_(chunkSize), numChunks_(numChunks), upstream_(upstream)
    {
        if (!upstream_) throw std::runtime_error("no upstream resource");

        if (chunkSize_ < 4) {
            std::cout << "This is just a warning, but for every size smaller than 8, we occupy 8 bytes anyway";
        }

        blockSize_ = chunkSize_ * numChunks_;
        freePointer_ = getMoreMemory();
    }

    void printUsedMemory()
    {
        std::cout << "Memory Allocated: " << chunksAllocated_ * chunkSize_ << std::endl;
    }

private:
    void* do_allocate(std::size_t size, std::size_t alignment) override
    {
        if (size > chunkSize_) return upstream_->allocate(size, alignment);

        if (freePointer_ == nullptr) freePointer_ = getMoreMemory();

        // now we can for sure allocate all the objects.
        Chunk* allocated = freePointer_;
        freePointer_ = freePointer_->next;
        chunksAllocated_++;
        return reinterpret_cast<void*>(allocated);
    }

    Chunk* getMoreMemory()
    { // allocate 1 block of chunks of memory
        Chunk* start = reinterpret_cast<Chunk*>(upstream_->allocate(blockSize_));
        Chunk* it = start;
        for (size_t i = 0; i < numChunks_ - 1; ++i) {
            it->next = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(it) + chunkSize_);
            it = it->next;
        }
        it->next = nullptr;
        chunksFree_ += numChunks_;
        return start;
    }

    void do_deallocate(void* ptr, std::size_t size, std::size_t alignment) override
    {
        if (size > chunkSize_) return upstream_->deallocate(ptr, size, alignment);

        Chunk* c = reinterpret_cast<Chunk*>(ptr);
        c->next = freePointer_;
        freePointer_ = c;
        chunksAllocated_--;
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        if (this == &other) return true;

        // TODO: implement this


        return false;
    }

private:
    std::pmr::memory_resource* upstream_{nullptr};
};

#endif // !_MEMORY_POOL_HEADER_HPP_
