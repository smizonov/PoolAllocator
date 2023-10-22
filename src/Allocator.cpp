#include <Allocator.h>

#include <iostream>
#include <atomic>

namespace memory {

namespace  {

using mo = std::memory_order;

}

Allocator::Allocator(size_t const block_count, size_t const block_size)
    : maxBlockCount_(block_count)
{
    start_ = static_cast<char*> (malloc(block_count * block_size + block_count * sizeof (Block)));
    if (!start_)
        throw std::bad_alloc();

    head_.store(new (start_) Block, mo::memory_order_relaxed);
    auto currentBlock = head_.load();
    for (size_t i = 1; i < block_count; ++i)
    {
        currentBlock->nextFreeBlock = new (start_ + i * (block_size + sizeof (Block))) Block;
        currentBlock = currentBlock->nextFreeBlock;
    }
    currentBlock->nextFreeBlock = head_.load(mo::memory_order_relaxed);
}

Allocator::Allocator(Allocator && src)
    : maxBlockCount_(src.maxBlockCount_)
{
    head_.store(src.head_.load(mo::memory_order_relaxed), mo::memory_order_relaxed);
    currentSize_.store(src.currentSize_.load(mo::memory_order_relaxed), mo::memory_order_relaxed);
    std::swap(start_, src.start_);

    src.head_.store(nullptr, mo::memory_order_relaxed);
    currentSize_.store(0, mo::memory_order_relaxed);
}

void* Allocator::allocate()
{
    using mo = std::memory_order;
    if (maxBlockCount_ <= currentSize_.fetch_add(1, mo::memory_order_relaxed))
        throw std::bad_alloc();

    auto current = head_.load(mo::memory_order_acquire);
    while(!head_.compare_exchange_strong(current, current->nextFreeBlock));

    return static_cast<void *>(++current);
}

void Allocator::deallocate(void* block)
{
    auto manager{ reinterpret_cast<Block*>(block) - 1 };
    manager->nextFreeBlock = head_.load(mo::memory_order_relaxed);
    while(!head_.compare_exchange_strong(manager->nextFreeBlock, manager));

    if (0 >= currentSize_.fetch_sub(1, mo::memory_order_relaxed))
        throw std::runtime_error("Allocator::deallocate: deallocate without allocation");
}

Allocator::~Allocator()
{
    free(start_);
}

}
