#include <atomic>
#include <stddef.h>

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
    std::atomic<Block*> topFreeBlock_{ nullptr };
    char * startAllocatedMemory_{ nullptr };
    int const maxBlockCount_;
    std::atomic<int> usedBlockCount_{0};
};

}
