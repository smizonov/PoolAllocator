#include <functional>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include <Allocator.h>
#include <Utils.h>

namespace  {
using namespace memory;

void allocDealloc(
        Allocator & allocator,
        int const blockSize)
{
    auto allocatedRef = allocator.allocate();
    utils::DataForComparation storedData(blockSize, allocatedRef);
    EXPECT_TRUE(storedData.dataIsEqual());
    allocator.deallocate(allocatedRef);
}

void multipleThreadAllocation(
        Allocator & allocator,
        int const blockSize)
{
    static constexpr auto iterationCount{ 1000 };
    for (int i = 0; i < iterationCount; ++i)
    {
        allocDealloc(
                allocator,
                blockSize);
    }
}

}

TEST(AllocatorStress, SimpleAllocation)
{
    static constexpr auto blockCount{10};
    static constexpr auto blockSize{10};
    static constexpr auto threadsCount{ blockCount };
    Allocator allocator(blockCount, blockSize);

    std::vector<std::thread> threads;
    for (size_t i = 0; i < threadsCount; ++i)
        threads.emplace_back(std::thread(multipleThreadAllocation,
                                         std::ref(allocator),
                                         blockSize));

    for (auto & thread : threads)
        thread.join();
}

