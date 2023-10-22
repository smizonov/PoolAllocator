#include <iostream>
#include <atomic>

namespace memory
{

struct Block
{
    Block* nextFreeBlock;
};

class Allocator
{
public:
    Allocator(size_t blockCount, size_t blockSize);

    Allocator(Allocator &&);
    Allocator(Allocator const &) = delete;
    Allocator& operator=(Allocator const &) = delete;
    Allocator& operator=(Allocator &&) = delete;
    ~Allocator();

public:
    void* allocate();
    void deallocate(void* block);

private:
    std::atomic<Block*> head_{ nullptr };
    int const maxBlockCount_;
    std::atomic<int> currentSize_{0};
    char * start_{ nullptr };
};

}
