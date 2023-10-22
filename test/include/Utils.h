#pragma once

#include <string>

namespace memory {
namespace utils {

std::string createRandStr(int const size);

struct DataForComparation {
    DataForComparation(
            size_t size,
            void * allocatedMemory);

    bool dataIsEqual();

private:
    char * dataInAllocator;
    std::string srcData;
};

}
}
