#include <Utils.h>

#include <cstring>
#include <string>

namespace memory {
namespace utils {

std::string createRandStr(int const size) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string dst;
    dst.reserve(size);

    for (int i = 0; i < size; ++i) {
        dst[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return dst;
}

DataForComparation::DataForComparation(
        size_t size,
        void * allocatedMemory)
{
    auto sizeOfDataExcludingNullTerminated{ size - 1 };
    srcData = createRandStr(sizeOfDataExcludingNullTerminated);
    dataInAllocator = static_cast<char *>(allocatedMemory);
    memcpy(dataInAllocator, srcData.c_str(), size);
}

bool DataForComparation::dataIsEqual()
{
    return !strcmp(dataInAllocator, srcData.c_str());
}

}
}
