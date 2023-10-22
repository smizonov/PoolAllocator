#include <Allocator.h>

#include <algorithm>
#include <stdexcept>

namespace memory {

namespace  {

using mo = std::memory_order;

}

Allocator::Allocator(size_t const block_count, size_t const block_size)
    : maxBlockCount_(block_count)
{
    startAllocatedMemory_ = static_cast<char*> (malloc(block_count * block_size + block_count * sizeof (Block)));
    if (!startAllocatedMemory_)
        throw std::bad_alloc();

    topFreeBlock_.store(new (startAllocatedMemory_) Block, mo::memory_order_relaxed);
    auto currentBlock = topFreeBlock_.load();
    for (size_t i = 1; i < block_count; ++i)
    {
        auto offset{ i * (block_size + sizeof (Block)) };
        currentBlock->nextFreeBlock = new (startAllocatedMemory_ + offset) Block;
        currentBlock = currentBlock->nextFreeBlock;
    }
    currentBlock->nextFreeBlock = topFreeBlock_.load(mo::memory_order_relaxed);
}

Allocator::Allocator(Allocator && src)
    : maxBlockCount_(src.maxBlockCount_)
{
    topFreeBlock_.store(src.topFreeBlock_.load(mo::memory_order_relaxed), mo::memory_order_relaxed);
    usedBlockCount_.store(src.usedBlockCount_.load(mo::memory_order_relaxed), mo::memory_order_relaxed);
    std::swap(startAllocatedMemory_, src.startAllocatedMemory_);

    src.topFreeBlock_.store(nullptr, mo::memory_order_relaxed);
    usedBlockCount_.store(0, mo::memory_order_relaxed);
}

void* Allocator::allocate()
{
    if (maxBlockCount_ <= usedBlockCount_.fetch_add(1, mo::memory_order_relaxed))
        throw std::bad_alloc();

    auto current = topFreeBlock_.load(mo::memory_order_acquire);
    while(!topFreeBlock_.compare_exchange_strong(current, current->nextFreeBlock));

    return static_cast<void *>(++current);
}

void Allocator::deallocate(void* block)
{
    auto current{ reinterpret_cast<Block*>(block) - 1 };
    current->nextFreeBlock = topFreeBlock_.load(mo::memory_order_relaxed);
    while(!topFreeBlock_.compare_exchange_strong(current->nextFreeBlock, current));

    if (0 >= usedBlockCount_.fetch_sub(1, mo::memory_order_relaxed))
        throw std::runtime_error("Allocator::deallocate: deallocate without allocation");
}

Allocator::~Allocator()
{
    free(startAllocatedMemory_);
}

}
