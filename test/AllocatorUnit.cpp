#include <string>

#include <gtest/gtest.h>

#include <Allocator.h>
#include <Utils.h>

TEST(AllocatorUnit, AllocDealloc)
{
    using namespace memory;
    static constexpr auto kBlockCount{10};
    static constexpr auto kBlockSize{10};
    Allocator allocator(kBlockCount, kBlockSize);
    std::vector<void*> allocatedRefs;
    for (int i = 0; i < kBlockCount; ++i)
    {
        allocatedRefs.push_back(allocator.allocate());
    }

    for (int i = 0; i < kBlockCount; ++i)
    {
        allocator.deallocate(allocatedRefs[i]);
    }
}

TEST(AllocatorUnit, ReverseDeallocation)
{
    using namespace memory;
    static constexpr auto kBlockCount{10};
    static constexpr auto kBlockSize{10};
    Allocator allocator(kBlockCount, kBlockSize);
    std::vector<void*> allocatedRefs;
    std::vector<utils::DataForComparation> storedData;
    for (int i = 0; i < kBlockCount; ++i)
    {
        allocatedRefs.push_back(allocator.allocate());
        storedData.emplace_back(utils::DataForComparation(kBlockSize, allocatedRefs.back()));
    }

    for (int i = kBlockCount - 1; i <= 0; --i)
    {
        EXPECT_TRUE(storedData[i].dataIsEqual());
        allocator.deallocate(allocatedRefs[i]);
    }
}

TEST(AllocatorUnit, MultipleFillAndDeallocateMemory)
{
    using namespace memory;
    static constexpr auto kBlockCount{1000};
    static constexpr auto kBlockSize{10};
    Allocator allocator(kBlockCount, kBlockSize);
    static constexpr auto iterationCount{ 10 };
    for (int totalIteration = 0; totalIteration < iterationCount; ++totalIteration)
    {
        for (int i = 0; i < kBlockCount; ++i)
        {
            std::vector<void*> allocatedRefs;
            std::vector<utils::DataForComparation> storedData;
            for (int j = 0; j < i; ++j)
            {
                allocatedRefs.push_back(allocator.allocate());
                storedData.push_back(utils::DataForComparation(kBlockSize, allocatedRefs.back()));
                EXPECT_TRUE(storedData[j].dataIsEqual());
                allocator.deallocate(allocatedRefs[j]);
            }
            allocatedRefs.clear();
            storedData.clear();
        }
    }
}
