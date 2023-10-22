#include <iostream>
#include <atomic>

using namespace std;

struct Block
{
    Block* nextFreeBlock;
};

class Allocator
{
public:
    void* allocate();
    void deallocate(void* block);
    Allocator(size_t block_count, size_t block_size);
    ~Allocator();

private:
    std::atomic<Block*> head_;
    int const maxSize_;
    std::atomic<int> currentSize_{0};
    char * start_;
};

Allocator::Allocator(size_t block_count, size_t block_size)
    : maxSize_(block_count)
{
    start_ = static_cast<char*> (malloc(block_count * block_size + block_count * sizeof (Block)));
    if (!start_)
        throw std::bad_alloc();

    head_.store(new (start_) Block);
    auto currentBlock = head_.load();
    for (size_t i = 1; i < block_count; ++i)
    {
        currentBlock->nextFreeBlock = new (start_ + i * block_size) Block;
        currentBlock = currentBlock->nextFreeBlock;
    }
    currentBlock->nextFreeBlock = head_.load();
}

void* Allocator::allocate()
{
    if (maxSize_ <= currentSize_.fetch_add(1))
        throw std::bad_alloc();

    auto current = head_.load();
    while(!head_.compare_exchange_strong(current, current->nextFreeBlock));
    return static_cast<void *>( current + sizeof (Block) );
}

void Allocator::deallocate(void* block)
{
    auto manager{ reinterpret_cast<Block*>(static_cast<char *>(block) - sizeof (Block)) };
    manager->nextFreeBlock = head_.load();
    while(!head_.compare_exchange_strong(manager->nextFreeBlock, manager));

    if (0 >= currentSize_.fetch_sub(1))
        throw std::runtime_error("Allocator::deallocate: deallocate without allocation");
}

Allocator::~Allocator()
{
    free(start_);
}

int main()
{
    cout << "Hello World!" << endl;
    return 0;
}
